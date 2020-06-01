#include <fstream>
#include <iostream>
#include <limits>
#include <pthread.h>
#include <semaphore.h>
#include <set>
#include <sstream>
#include <vector>

struct Job {
  int id;
  std::set<int> dependency;
  int left;
  std::vector<std::string> right;
};

int main(int argc, char **argv) {
  // get args
  int thread_nums = atoi(argv[1]);
  std::string outfile = argv[2];

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
    bool is_left = true;
    bool is_right = false;
    std::stringstream ss(line);
    std::string token;

    job.id = job_idx;

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

  std::vector<bool> job_done(job_list.size(), false);

  return 0;
}
