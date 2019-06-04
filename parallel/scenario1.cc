#include <cmath>
#include <random>
#include <sstream>
#include "thpool1/api.h"
#include "scenario.h"


scenario1::scenario1(size_t n, size_t seed) {
  input_data.resize(n);
  output_data.resize(n);

  std::mt19937 gen{ static_cast<uint32_t>(seed) };
  std::normal_distribution<> normal_distribution(0.0);

  for (size_t i = 0; i < n; ++i) {
    input_data[i] = normal_distribution(gen);
  }
}


std::string scenario1::name() {
  std::ostringstream ss;
  ss << "Computing sin(X), where X.size = " << input_data.size();
  return ss.str();
}


void scenario1::run_omp() {
  size_t n = input_data.size();
  const double* inputs = input_data.data();
  double* outputs = output_data.data();

  #pragma omp parallel for schedule(static) num_threads(nthreads)
  for (size_t i = 0; i < n; ++i) {
    outputs[i] = std::sin(inputs[i]);
  }
}


void scenario1::run_thpool1() {
  size_t n = input_data.size();
  const double* inputs = input_data.data();
  double* outputs = output_data.data();

  dt1::parallel_for_static(
    /* nrows = */ n,
    /* min_chunk_size = */ 1024,
    /* nthreads = */ nthreads,
    [&](size_t i) {
      outputs[i] = std::sin(inputs[i]);
    });
}
