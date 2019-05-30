#include <getopt.h>  // getopt
#include <unistd.h>  // getopt_long
#include <stdlib.h>  // atol, atoi, atof
#include "threadpool/api.h"
#include "threadpool/thread_pool.h"
#include "scenario.h"


struct config {
  size_t seed;
  size_t n;
  double time;
  int nthreads;
  int : 32;

  config() {
    seed = 1;
    n = 1000000;
    time = 1.0;
    nthreads = dt::get_hardware_concurrency();
  }

  void parse(int argc, char** argv) {
    struct option longopts[] = {
      {"seed", 1, 0, 0},
      {"n", 1, 0, 0},
      {"nthreads", 1, 0, 0},
      {"time", 1, 0, 0},
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
    printf("\n");
  }
};



int main(int argc, char** argv) {
  // Parsing input parameters...
  config cfg;
  cfg.parse(argc, argv);
  cfg.report();

  dt::thread_pool::get_instance()->resize(dt::get_hardware_concurrency());


  //
  auto sc = scenptr(new scenario1(cfg.n, cfg.seed));
  sc->set_nthreads(cfg.nthreads);
  sc->set_max_time(cfg.time);
  sc->benchmark();
}
