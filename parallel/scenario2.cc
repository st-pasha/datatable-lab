#include <cmath>
#include <random>
#include <sstream>
#include "threadpool/api.h"
#include "scenario.h"


scenario2::scenario2(size_t n_)
  : n(n_) {}


std::string scenario2::name() {
  std::ostringstream ss;
  ss << "parallel region { " << n << " iterations inside }";
  return ss.str();
}


void scenario2::run_omp() {
  #pragma omp parallel num_threads(nthreads)
  {
    volatile double res = 0.0;
    for (size_t i = 0; i < n; ++i) {
      res += std::sin(1.0 * i);
    }
  }
}


void scenario2::run_threadpool() {
  size_t nn = this->n;
  dt::parallel_region(
    /* nthreads = */ nthreads,
    [nn]{
      volatile double res = 0.0;
      for (size_t i = 0; i < nn; ++i) {
        res += std::sin(1.0 * i);
      }
    });
}
