#include <chrono>
#include "scenario.h"



scenario::~scenario() {}


template <typename F>
static double timeit(F fun) {
  auto t0 = std::chrono::high_resolution_clock::now();
  fun();
  auto t1 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> delta = t1 - t0;
  return delta.count();
}


double scenario::time_omp() {
  return timeit([&]{ run_omp(); });
}

double scenario::time_threadpool() {
  return timeit([&]{ run_threadpool(); });
}

