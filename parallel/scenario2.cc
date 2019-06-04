#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <omp.h>
#include "thpool1/api.h"
#include "scenario.h"

static void summarize(std::vector<double>& times) {
  size_t n = times.size();
  std::sort(times.begin(), times.end());
  // if (n > 10) {
  //   n = static_cast<size_t>(0.95 * n);
  //   times.resize(n);
  // }

  double scale = 1.0;
  std::string unit = "s";
  // if (times.back() < 1e-6) {
  //   scale = 1e9;
  //   unit = "ns";
  // } else if (times.back() < 0.001) {
  //   scale = 1e6;
  //   unit = "us";
  // } else if (times.back() < 1) {
    scale = 1e3;
    unit = "ms";
  // }

  size_t n2 = n / 2;
  double min_time = times[0] * scale;
  double max_time = times[n-1] * scale;
  double med_time = ((n&1)? times[n2] : 0.5*(times[n2] + times[n2-1])) * scale;
  double avg_time = std::accumulate(times.begin(), times.end(), 0.0)/n * scale;

  std::cout << std::fixed << std::setprecision(3);
  std::cout << avg_time << unit
            << " (" << min_time
            << "/" << med_time
            << "/" << max_time << ")";
}




scenario2::scenario2(size_t n_)
  : n(n_) {}


void scenario2::setup() {
  timings.resize(static_cast<size_t>(nthreads + 1));
  size_t m = std::max(1000000000 / n, 1ul);
  for (auto& tt : timings) {
    tt.reserve(m);
  }
}

void scenario2::teardown() {
  std::vector<double> all_starts, all_spans, all_ends;
  for (size_t i = 1; i < timings.size(); ++i) {
    std::cout << "  [" << std::setw(2) << (i-1) << "] ";
    size_t n = timings[i].size();
    std::vector<double> starts, spans, ends;
    for (size_t j = 0; j < n; ++j) {
      std::chrono::duration<double> start = timings[i][j].start - timings[0][j].start;
      std::chrono::duration<double> span  = timings[i][j].end - timings[i][j].start;
      std::chrono::duration<double> end   = timings[0][j].end - timings[i][j].end;
      starts.push_back(start.count());
      spans.push_back(span.count());
      ends.push_back(end.count());
    }
    all_starts.insert(all_starts.end(), starts.begin(), starts.end());
    all_spans.insert(all_spans.end(), spans.begin(), spans.end());
    all_ends.insert(all_ends.end(), ends.begin(), ends.end());
    std::cout << "start: "; summarize(starts);
    std::cout << ", span: "; summarize(spans);
    std::cout << ", end: "; summarize(ends);
    std::cout << "\n";
    timings[i].clear();
  }
  std::cout << "  All: ";
  std::cout << "start: "; summarize(all_starts);
  std::cout << ", span: "; summarize(all_spans);
  std::cout << ", end: "; summarize(all_ends);
  std::cout << "\n\n";
  timings[0].clear();
}


std::string scenario2::name() {
  std::ostringstream ss;
  ss << "parallel region { " << n << " iterations inside }";
  return ss.str();
}


void scenario2::run_omp() {
  auto g0 = std::chrono::high_resolution_clock::now();
  #pragma omp parallel num_threads(nthreads)
  {
    auto t0 = std::chrono::high_resolution_clock::now();
    size_t ith = static_cast<size_t>(omp_get_thread_num());
    volatile double res = 0.0;
    for (size_t i = 0; i < n; ++i) {
      res += std::sin(1.0 * i);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    assert(ith + 1 < timings.size());
    timings[ith + 1].push_back({t0, t1});
  }
  auto g1 = std::chrono::high_resolution_clock::now();
  timings[0].push_back({g0, g1});
}


void scenario2::run_thpool1() {
  auto g0 = std::chrono::high_resolution_clock::now();
  dt1::parallel_region(
    /* nthreads = */ nthreads,
    [=]{
      auto t0 = std::chrono::high_resolution_clock::now();
      size_t ith = dt1::this_thread_index();
      volatile double res = 0.0;
      for (size_t i = 0; i < n; ++i) {
        res += std::sin(1.0 * i);
      }
      auto t1 = std::chrono::high_resolution_clock::now();
      timings[ith + 1].push_back({t0, t1});
    });
  auto g1 = std::chrono::high_resolution_clock::now();
  timings[0].push_back({g0, g1});
}
