#include <fstream>
#include <iostream>
#include <limits>
#include <pthread.h>
#include <semaphore.h>
#include <sstream>
#include <unordered_set>
#include <vector>

sem_t is_job_ready, is_job_done;
sem_t mutex_pending_job, mutex_pending_var;

struct Job {
  int id;
  bool is_taken;
  int left;
  std::vector<std::string> right;
};

struct Arg {
  std::vector<int> *nums;
  std::vector<struct Job> *pending_job;
  std::vector<bool> *pending_var;
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
                    std::vector<bool> *pending_var) {
  sem_wait(&mutex_pending_var);
  sem_wait(&mutex_pending_job);

  // find non-taken job, and mark that job as taken
  int idx = 0;
  for (; idx < pending_job->size(); idx++)
    if (pending_job->at(idx).is_taken == false)
      break;
  int var = pending_job->at(idx).left;
  pending_job->at(idx).is_taken = true;

  // do the job
  thread_worker(nums, pending_job->at(idx));

  pending_var->at(var) = false;

  sem_post(&mutex_pending_job);
  sem_post(&mutex_pending_var);
}

void *thread_pool_manager(void *void_arg) {
  Arg *arg = (Arg *)void_arg;
  for (;;) {
    sem_wait(&is_job_ready);
    thread_manager(arg->nums, arg->pending_job, arg->pending_var);
    sem_post(&is_job_done);
  }
}

void job_dispatcher(std::vector<struct Job> &job_list,
                    std::vector<struct Job> &pending_job,
                    std::vector<bool> &pending_var) {
  // send possible jobs
  for (; pending_job.size() < job_list.size();) {
    sem_wait(&mutex_pending_var);
    sem_wait(&mutex_pending_job);
    for (int i = 0; i < job_list.size(); i++) {
      // check whether job already pending
      if (job_list.at(i).is_taken)
        continue;

      if (pending_var.at(job_list.at(i).left) == false) {
        // send job into pending list
        pending_job.push_back(job_list.at(i));
        sem_post(&is_job_ready);

        job_list.at(i).is_taken = true;
        pending_var.at(job_list.at(i).left) = true;
      }
    }
    sem_post(&mutex_pending_job);
    sem_post(&mutex_pending_var);
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
  int job_idx = 0;
  std::string line;
  std::getline(std::cin, line); // read line break
  while (std::getline(std::cin, line)) {
    struct Job job;
    job.id = job_idx;
    job.is_taken = false;
    bool is_left = true;
    bool is_right = false;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ' ')) {
      if (is_right) {
        job.right.push_back(token);
      } else {
        if (token == "=")
          is_right = true;
      }
      if (is_left) {
        int left_var = stoi(token.substr(1, token.size() - 1));
        job.left = left_var;
        is_left = false;
      }
    }

    // push job into job list
    job_list.push_back(job);
    job_idx++;
  }

  sem_init(&is_job_ready, 0, 0);
  sem_init(&is_job_done, 0, 0);
  sem_init(&mutex_pending_job, 0, 1);
  sem_init(&mutex_pending_var, 0, 1);

  std::vector<pthread_t> tid(thread_nums);
  std::vector<struct Arg> args(thread_nums);
  std::vector<struct Job> pending_job;
  std::vector<bool> pending_var(var_num, false);

  // create a new thread pool
  for (int i = 0; i < thread_nums; i++) {
    args.at(i).nums = &var_vec;
    args.at(i).pending_job = &pending_job;
    args.at(i).pending_var = &pending_var;
    pthread_create(&tid.at(i), nullptr, thread_pool_manager, &args.at(i));
  }

  job_dispatcher(job_list, pending_job, pending_var);

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
