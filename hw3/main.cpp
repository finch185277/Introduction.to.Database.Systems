#include <fstream>
#include <iostream>
#include <limits>
#include <pthread.h>
#include <semaphore.h>
#include <set>
#include <sstream>
#include <vector>

sem_t is_job_ready, is_job_done;
sem_t mutux_pending_job, mutux_done_list;

struct Job {
  int id;
  bool is_taken;
  std::set<int> dependency;
  int left;
  std::vector<std::string> right;
};

struct Arg {
  std::vector<int> *nums;
  std::vector<struct Job> *pending_job;
  std::vector<bool> *done_list;
};

void job_worker(std::vector<int> *nums, struct Job &job) {
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

void job_manager(std::vector<int> *nums, std::vector<struct Job> *pending_job,
                 std::vector<bool> *done_list) {
  // find non-taken job, and mark that job as taken
  sem_wait(&mutux_pending_job);
  int idx = 0;
  for (; idx < pending_job->size(); idx++)
    if (pending_job->at(idx).is_taken == false)
      break;
  // if (idx == pending_job->size())
  //   std::cout << "pending job index fail!!" << std::endl;
  int job_id = pending_job->at(idx).id;
  pending_job->at(idx).is_taken = true;
  sem_post(&mutux_pending_job);

  // do the job
  job_worker(nums, pending_job->at(idx));

  // mark complete job as done
  sem_wait(&mutux_done_list);
  done_list->at(job_id) = true;
  sem_post(&mutux_done_list);
}

void *thread_pool_manager(void *void_arg) {
  Arg *arg = (Arg *)void_arg;
  for (;;) {
    sem_wait(&is_job_ready);
    job_manager(arg->nums, arg->pending_job, arg->done_list);
    sem_post(&is_job_done);
  }
}

void job_dispatcher(std::vector<struct Job> &job_list,
                    std::vector<struct Job> &pending_job,
                    std::vector<bool> &done_list) {
  // send every possible job into pending list
  int first_round_job = 0;
  for (int i = 0; i < job_list.size(); i++) {
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
    sem_wait(&mutux_pending_job);
    pending_job.push_back(job_list.at(i));
    sem_post(&mutux_pending_job);
    sem_post(&is_job_ready);

    job_list.at(i).is_taken = true;
    first_round_job++;
  }

  // get job done semaphore and send one possible job
  for (int remain_jobs = job_list.size() - first_round_job; remain_jobs > 0;) {
    sem_wait(&is_job_done);
    sem_wait(&mutux_done_list);
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
      sem_wait(&mutux_pending_job);
      pending_job.push_back(job_list.at(i));
      sem_post(&mutux_pending_job);
      sem_post(&is_job_ready);

      job_list.at(i).is_taken = true;
      remain_jobs--;
      break;
    }
    sem_post(&mutux_done_list);
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
  std::vector<int> dep_list(var_num, -1);
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
        is_left = false;
      } else {
        if (token[0] == '$') {
          int right_var = stoi(token.substr(1, token.size() - 1));
          if (dep_list.at(right_var) != -1)
            job.dependency.insert(dep_list.at(right_var));
        }
      }
    }
    dep_list.at(job.left) = job_idx;
    job_list.push_back(job);
    job_idx++;
  }

  // check job list
  // std::cout << "check job list!!" << std::endl;
  // for (int i = 0; i < job_list.size(); i++) {
  //   std::cout << job_list.at(i).left << " -> ";
  //   for (int j = 0; j < job_list.at(i).right.size(); j++) {
  //     std::cout << job_list.at(i).right.at(j) << " ";
  //   }
  //   std::cout << "; dep: ";
  //   for (auto dep : job_list.at(i).dependency) {
  //     std::cout << dep << " ";
  //   }
  //   std::cout << std::endl;
  // }

  std::vector<pthread_t> tid(thread_nums);
  std::vector<struct Arg> args(thread_nums);
  std::vector<struct Job> pending_job;
  std::vector<bool> done_list(job_list.size(), false);

  sem_init(&is_job_ready, 0, 0);
  sem_init(&is_job_done, 0, 0);
  sem_init(&mutux_pending_job, 0, 1);
  sem_init(&mutux_done_list, 0, 1);

  // create a new thread pool
  for (int i = 0; i < thread_nums; i++) {
    args.at(i).nums = &var_vec;
    args.at(i).pending_job = &pending_job;
    args.at(i).done_list = &done_list;
    pthread_create(&tid.at(i), nullptr, thread_pool_manager, &args.at(i));
  }

  job_dispatcher(job_list, pending_job, done_list);

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
