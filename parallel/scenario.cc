#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
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

  int n2 = n_runs / 2;
  double med_time = (n_runs & 1)? durations[n2]
                                : 0.5*(durations[n2] + durations[n2 - 1]);
  double min_time = durations.front();
  double max_time = durations.back();
  double avg_time = total_time / n_runs;

  std::string units = "s";
  if (max_time < 1) {
    med_time *= 1000.0;
    min_time *= 1000.0;
    max_time *= 1000.0;
    avg_time *= 1000.0;
    units = "ms";
  }

  std::cout << std::fixed << std::setw(8) << std::setprecision(3);
  std::cout << avg_time << units
            << " (min=" << min_time << units
            << ", med=" << med_time << units
            << ", max=" << max_time << units
            << ")\n";
}


void scenario::benchmark(int backends) {
  std::cout << "\nBenchmarking " << name() << "\n";
  if (backends & Backend::OMP) {
    benchmarkit("OMP       ", [&]{ run_omp(); });
  }
  if (backends & Backend::THP) {
    benchmarkit("ThreadPool", [&]{ run_threadpool(); });
  }
}

