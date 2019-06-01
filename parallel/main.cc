#define _GNU_SOURCE
#include <iostream>
#include <getopt.h>  // getopt
#include <unistd.h>  // getopt_long
#include <stdlib.h>  // atol, atoi, atof
#include <time.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <pthread.h>
#include "threadpool/api.h"
#include "threadpool/thread_pool.h"
#include "scenario.h"
#ifdef __APPLE__
#include <mach/thread_policy.h>
#include <mach/thread_act.h>
#include <mach/mach_init.h>
#endif


struct config {
  size_t seed;
  size_t n;
  double time;
  int nthreads;
  int task;

  config() {
    seed = 1;
    n = 1000000;
    time = 1.0;
    nthreads = dt::get_hardware_concurrency();
    task = 1;
  }

  void parse(int argc, char** argv) {
    struct option longopts[] = {
      {"seed", 1, 0, 0},
      {"n", 1, 0, 0},
      {"nthreads", 1, 0, 0},
      {"time", 1, 0, 0},
      {"task", 1, 0, 0},
      {nullptr, 0, nullptr, 0}  // sentinel
    };

    while (1) {
      int option_index;
      int ret = getopt_long(argc, argv, "", longopts, &option_index);
      if (ret == -1) break;
      if (ret == 0) {
        if (optarg) {
          if (option_index == 0) seed = atol(optarg);
          if (option_index == 1) n = atol(optarg);
          if (option_index == 2) nthreads = atoi(optarg);
          if (option_index == 3) time = atof(optarg);
          if (option_index == 4) task = atoi(optarg);
        }
      }
    }
  }

  void report() {
    printf("\nInput parameters:\n");
    printf("  seed     = %zu\n", seed);
    printf("  n        = %zu\n", n);
    printf("  time     = %.1f\n", time);
    printf("  nthreads = %d\n", nthreads);
    printf("  task     = %d\n", task);
    printf("\n");
  }
};



int main(int argc, char** argv) {
  // Parsing input parameters...
  config cfg;
  cfg.parse(argc, argv);

  dt::thread_pool::get_instance()->resize(dt::get_hardware_concurrency());

  // Check process limits
  int policy;
  struct sched_param params;
  pthread_getschedparam(pthread_self(), &policy, &params);
  std::cout << "Policy: " << (policy==SCHED_FIFO? "SCHED_FIFO" :
                              policy==SCHED_RR? "SCHED_RR" :
                              policy==SCHED_OTHER? "SCHED_OTHER" : "?") << "\n";
  std::cout << "Priority: " << params.sched_priority << "\n";
  int prmin = sched_get_priority_min(policy);
  int prmax = sched_get_priority_max(policy);
  std::cout << "Priorities range: " << prmin << ".." << prmax << "\n";
  // cpu_set_t cpuset;
  // int ret = pthread_getaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
  // std::cout << "Affinity: ";
  // for (int i = 0; i < CPU_SETSIZE; ++i) std::cout << (int)(CPU_ISSET(i));
  // std::cout << "\n";
  #ifdef __APPLE__
    struct thread_affinity_policy pol;
    mach_port_t thread = mach_thread_self();
    boolean_t get_default = false;
    mach_msg_type_number_t mmtn = THREAD_AFFINITY_POLICY_COUNT;
    int ret = thread_policy_get(thread,
                                THREAD_AFFINITY_POLICY,
                                (thread_policy_t)&pol,
                                &mmtn,
                                &get_default);
    std::cout << "ret=" << ret << ", policy=" << pol.affinity_tag << ", mach_msg_type_number=" << mmtn << "\n";
  #endif

  //
  auto sc = cfg.task == 1? scenptr(new scenario1(cfg.n, cfg.seed)) :
            cfg.task == 2? scenptr(new scenario2(cfg.n)) :
            scenptr(nullptr);
  if (sc) {
    sc->set_nthreads(cfg.nthreads);
    sc->set_max_time(cfg.time);
    sc->benchmark();
  }
}
