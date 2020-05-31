#include <fstream>
#include <iostream>
#include <limits>
#include <pthread.h>
#include <semaphore.h>
#include <sstream>
#include <vector>

struct Job {
  int dependency;
  std::vector<std::string> content;
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
  std::string line;
  while (std::getline(std::cin, line)) {
    std::stringstream ss(line);
    std::string token;
    struct Job job;
    while (std::getline(ss, token, ' ')) {
      job.content.push_back(token);
    }
    job_list.push_back(job);
  }

  // check job list
  // for (int i = 0; i < job_list.size(); i++) {
  //   for (int j = 0; j < job_list.at(i).content.size(); j++) {
  //     std::cout << job_list.at(i).content.at(j) << " ";
  //   }
  //   std::cout << std::endl;
  // }

  return 0;
}
