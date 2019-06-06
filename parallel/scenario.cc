#include <algorithm>
#include <chrono>
#include <cmath>       // std::pow, std::sqrt
#include <iomanip>
#include <iostream>
#include <numeric>     // std::accumulate
#include <thread>
#include "thpool1/thread_pool.h"
#include "thpool2/thread_pool.h"
#include "thpool3/thread_pool.h"
#include "scenario.h"


scenario::scenario() {
  max_time = 1.0;
  nthreads = dt1::get_hardware_concurrency();
  backends = Backend::OMP | Backend::TP1 | Backend::TP2 | Backend::TP3;
}

scenario::~scenario() {}

void scenario::set_nthreads(int nth) {
  nthreads = nth;
}

void scenario::set_backends(int b) {
  backends = b;
}

void scenario::set_max_time(double t) {
  max_time = t;
}

void scenario::setup() {}
void scenario::teardown() {}


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
  std::cout << "  \x1B[1m" << backend_name << "\x1B[m: ";
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

static void startup_thpool1() {
  std::mutex m;
  std::vector<int> threadnums;
  dt1::parallel_region(
    [&]{
      size_t i = dt1::this_thread_index();
      std::lock_guard<std::mutex> lock(m);
      threadnums.push_back(omp_get_thread_num());
    });
}

static void stop_thpool1() {
  dt1::thread_pool::get_instance()->resize(0);
}

static void startup_thpool2() {
  std::mutex m;
  std::vector<int> threadnums;
  dt2::parallel_region(
    [&]{
      size_t i = dt2::this_thread_index();
      std::lock_guard<std::mutex> lock(m);
      threadnums.push_back(omp_get_thread_num());
    });
}

static void stop_thpool2() {
  dt2::thpool->resize(1);
}

static void startup_thpool3() {
  std::mutex m;
  std::vector<int> threadnums;
  dt3::parallel_region(
    [&]{
      size_t i = dt3::this_thread_index();
      std::lock_guard<std::mutex> lock(m);
      threadnums.push_back(omp_get_thread_num());
    });
}

static void stop_thpool3() {
  dt3::thpool->resize(1);
}

void scenario::benchmark() {
  std::cout << "Benchmarking [nthreads=" << nthreads << "] " << name() << "\n";
  if (backends & Backend::TP1) {
    startup_thpool1();
    setup();
    benchmarkit("ThPool1", [&]{ run_thpool1(); }, max_time);
    teardown();
    stop_thpool1();
  }
  if (backends & Backend::TP2) {
    startup_thpool2();
    setup();
    benchmarkit("ThPool2", [&]{ run_thpool2(); }, max_time);
    teardown();
    stop_thpool2();
  }
  if (backends & Backend::TP3) {
    startup_thpool3();
    setup();
    benchmarkit("ThPool3", [&]{ run_thpool3(); }, max_time);
    teardown();
    stop_thpool3();
  }
  if (backends & Backend::OMP) {
    startup_omp();
    setup();
    benchmarkit("OMP    ", [&]{ run_omp(); }, max_time);
    teardown();
  }
}

