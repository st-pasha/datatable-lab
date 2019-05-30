#include <algorithm>
#include <chrono>
#include <cmath>       // std::pow, std::sqrt
#include <iomanip>
#include <iostream>
#include <numeric>     // std::accumulate
#include <omp.h>
#include "threadpool/api.h"
#include "scenario.h"


scenario::scenario() {
  max_time = 1.0;
  nthreads = dt::get_hardware_concurrency();
}

scenario::~scenario() {}

void scenario::set_nthreads(int nth) {
  nthreads = nth;
}

void scenario::set_max_time(double t) {
  max_time = t;
}


template <typename F>
static double timeit(F fun) {
  auto t0 = std::chrono::high_resolution_clock::now();
  fun();
  auto t1 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> delta = t1 - t0;
  return delta.count();
}


template <typename F>
static void benchmarkit(const std::string& backend_name, F fun,
                        double max_runtime = 1.0)
{
  std::cout << "  " << backend_name << ": ";
  std::vector<double> durations;
  double total_time = 0.0;
  int n_runs = 0;
  while (total_time < max_runtime) {
    double duration = timeit(fun);
    durations.push_back(duration);
    total_time += duration;
    n_runs ++;
  }
  std::sort(durations.begin(), durations.end());
  if (n_runs >= 10) {
    n_runs = static_cast<size_t>(n_runs * 0.95);
    durations.resize(n_runs);
  }

  int n2 = n_runs / 2;
  double med_time = (n_runs & 1)? durations[n2]
                                : 0.5*(durations[n2] + durations[n2 - 1]);
  double min_time = durations.front();
  double max_time = durations.back();
  double avg_time = std::accumulate(durations.begin(), durations.end(), 0.0) / n_runs;
  double ssq_time = std::accumulate(durations.begin(), durations.end(), 0.0,
                                    [avg_time](double acc, double val){
                                      return acc + std::pow(val - avg_time, 2);
                                    });
  double stdev_time = std::sqrt(ssq_time / n_runs);

  std::string units = "s";
  if (max_time < 1) {
    med_time *= 1000.0;
    min_time *= 1000.0;
    max_time *= 1000.0;
    avg_time *= 1000.0;
    stdev_time *= 1000.0;
    units = "ms";
  }

  std::cout << std::fixed << std::setw(8) << std::setprecision(3);
  std::cout << avg_time << units
            << " ± " << stdev_time
            << " (min=" << min_time << units
            << ", med=" << med_time << units
            << ", max=" << max_time << units
            << ", n=" << n_runs
            << ")\n";
}


static void startup_omp() {
  std::vector<int> threadnums;
  #pragma omp parallel
  {
    #pragma omp critical
    {
      threadnums.push_back(omp_get_thread_num());
    }
  }
}



void scenario::benchmark(int backends) {
  std::cout << "Benchmarking [nthreads=" << nthreads << "] " << name() << "\n";
  if (backends & Backend::OMP) {
    startup_omp();
    benchmarkit("OMP       ", [&]{ run_omp(); }, max_time);
  }
  if (backends & Backend::THP) {
    benchmarkit("ThreadPool", [&]{ run_threadpool(); }, max_time);
  }
}

