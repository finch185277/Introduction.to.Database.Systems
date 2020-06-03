#include <fstream>
#include <iostream>
#include <limits>
#include <pthread.h>
#include <semaphore.h>
#include <sstream>
#include <sys/time.h>
#include <unordered_set>
#include <vector>

sem_t is_job_ready, is_job_done;
sem_t mutex_pending_job, mutex_done_list;

struct Job {
  int id;
  bool is_taken;
  std::unordered_set<int> dependency;
  int left;
  std::vector<std::string> right;
};

struct Arg {
  std::vector<int> *nums;
  std::vector<struct Job> *pending_job;
  std::vector<bool> *done_list;
};

void thread_worker(std::vector<int> *nums, struct Job &job) {
  int result = 0;
  bool is_positive = true;
  for (int i = 0; i < job.right.size(); i++) {
    if (job.right.at(i)[0] == '$') {
      int var_idx = stoi(job.right.at(i).substr(1, job.right.at(i).size() - 1));
      if (is_positive)
        result += nums->at(var_idx);
      else
        result -= nums->at(var_idx);
    } else if (job.right.at(i) == "+") {
      is_positive = true;
    } else if (job.right.at(i) == "-") {
      is_positive = false;
    } else {
      if (is_positive)
        result += stoi(job.right.at(i));
      else
        result -= stoi(job.right.at(i));
    }
  }
  nums->at(job.left) = result;
}

void thread_manager(std::vector<int> *nums,
                    std::vector<struct Job> *pending_job,
                    std::vector<bool> *done_list) {
  sem_wait(&mutex_done_list);
  sem_wait(&mutex_pending_job);
  // find non-taken job, and mark that job as taken
  int idx = 0;
  for (; idx < pending_job->size(); idx++)
    if (pending_job->at(idx).is_taken == false)
      break;
  int job_id = pending_job->at(idx).id;
  pending_job->at(idx).is_taken = true;

  // do the job
  thread_worker(nums, pending_job->at(idx));

  // mark complete job as done
  done_list->at(job_id) = true;
  sem_post(&mutex_pending_job);
  sem_post(&mutex_done_list);
}

void *thread_pool_manager(void *void_arg) {
  Arg *arg = (Arg *)void_arg;
  for (;;) {
    sem_wait(&is_job_ready);
    thread_manager(arg->nums, arg->pending_job, arg->done_list);
    sem_post(&is_job_done);
  }
}

void job_dispatcher(std::vector<struct Job> &job_list,
                    std::vector<struct Job> &pending_job,
                    std::vector<bool> &done_list) {
  // send every possible job into pending list
  sem_wait(&mutex_pending_job);
  for (int i = 0; i < job_list.size(); i++) {
    // check whether depend on other jobs
    if (!job_list.at(i).dependency.empty())
      continue;

    // send job into pending list
    pending_job.push_back(job_list.at(i));
    sem_post(&is_job_ready);

    job_list.at(i).is_taken = true;
  }
  sem_post(&mutex_pending_job);

  // send possible jobs
  for (; pending_job.size() < job_list.size();) {
    sem_wait(&mutex_done_list);
    sem_wait(&mutex_pending_job);
    for (int i = 0; i < job_list.size(); i++) {
      // check whether job already pending
      if (job_list.at(i).is_taken)
        continue;

      // check whether need wait other jobs done
      bool is_unsolved_dep = false;
      for (auto dep : job_list.at(i).dependency) {
        if (!done_list.at(dep)) {
          is_unsolved_dep = true;
          break;
        }
      }
      if (is_unsolved_dep)
        continue;

      // send job into pending list
      pending_job.push_back(job_list.at(i));
      sem_post(&is_job_ready);

      job_list.at(i).is_taken = true;
    }
    sem_post(&mutex_pending_job);
    sem_post(&mutex_done_list);
  }

  // wait all jobs done
  for (int remain_jobs = job_list.size(); remain_jobs > 0; remain_jobs--) {
    sem_wait(&is_job_done);
  }
}

int main(int argc, char **argv) {
  // get args
  int thread_nums = atoi(argv[1]);
  std::string file_name = argv[2];

  // read number of valuables
  int var_num;
  std::cin >> var_num;

  // initialize valuables
  std::vector<int> var_vec(var_num);
  for (int i = 0; i < var_num; i++)
    std::cin >> var_vec.at(i);

  // build job list
  std::vector<Job> job_list;
  std::vector<std::unordered_set<int>> dep_list(var_num);
  int job_idx = 0;
  std::string line;
  std::getline(std::cin, line); // read line break
  while (std::getline(std::cin, line)) {
    struct Job job;
    job.id = job_idx;
    job.is_taken = false;
    bool is_left = true;
    bool is_right = false;
    std::unordered_set<int> job_var_vec;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ' ')) {
      // store tokens
      if (is_right) {
        job.right.push_back(token);
      } else {
        if (token == "=")
          is_right = true;
      }
      // set dependency
      if (is_left) {
        int left_var = stoi(token.substr(1, token.size() - 1));
        job.left = left_var;
        job_var_vec.insert(left_var);
        if (dep_list.at(left_var).size() > 0)
          for (auto job_id : dep_list.at(left_var))
            job.dependency.insert(job_id);
        is_left = false;
      } else {
        if (token[0] == '$') {
          int right_var = stoi(token.substr(1, token.size() - 1));
          job_var_vec.insert(right_var);
          if (dep_list.at(right_var).size() > 0)
            for (auto job_id : dep_list.at(right_var))
              job.dependency.insert(job_id);
        }
      }
    }
    // push job id into dep list
    for (int var : job_var_vec)
      dep_list.at(var).insert(job_idx);

    // push job into job list
    job_list.push_back(job);
    job_idx++;
  }

  sem_init(&is_job_ready, 0, 0);
  sem_init(&is_job_done, 0, 0);
  sem_init(&mutex_pending_job, 0, 1);
  sem_init(&mutex_done_list, 0, 1);

  std::vector<pthread_t> tid(thread_nums);
  std::vector<struct Arg> args(thread_nums);
  std::vector<struct Job> pending_job;
  std::vector<bool> done_list(job_list.size(), false);

  // create a new thread pool
  for (int i = 0; i < thread_nums; i++) {
    args.at(i).nums = &var_vec;
    args.at(i).pending_job = &pending_job;
    args.at(i).done_list = &done_list;
    pthread_create(&tid.at(i), nullptr, thread_pool_manager, &args.at(i));
  }

  // start of count the time
  struct timeval st_start, st_end;
  gettimeofday(&st_start, 0);

  job_dispatcher(job_list, pending_job, done_list);

  // end of count the time
  gettimeofday(&st_end, 0);
  double st_sec = st_end.tv_sec - st_start.tv_sec;
  double st_usec = st_end.tv_usec - st_start.tv_usec;
  std::cout << "The process used: " << st_sec + (st_usec / 1000000)
            << " secs\n";

  // output result
  std::ofstream outfile(file_name);
  for (int i = 0; i < var_vec.size(); i++)
    outfile << "$" << i << " = " << var_vec.at(i) << std::endl;

  // delete the thread pool
  for (int i = 0; i < thread_nums; i++) {
    pthread_cancel(tid.at(i));
  }

  return 0;
}
