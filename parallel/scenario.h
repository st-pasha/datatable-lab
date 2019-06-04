#ifndef SCENARIO_h
#define SCENARIO_h
#include <chrono>
#include <memory>
#include <string>
#include <vector>

enum Backend {
  OMP = 1,
  TP1 = 2,
  TP2 = 3,
};


class scenario {
  protected:
    double max_time;
    int nthreads;
    int : 32;

  public:
    scenario();
    virtual ~scenario();

    void set_nthreads(int nth);
    void set_max_time(double t);
    virtual void setup();
    void benchmark(int backends = Backend::OMP | Backend::TP1 | Backend::TP2);
    virtual void teardown();

  protected:
    virtual std::string name() = 0;
    virtual void run_omp() = 0;
    virtual void run_thpool1() = 0;
    virtual void run_thpool2() = 0;
};

using scenptr = std::unique_ptr<scenario>;



class scenario1 : public scenario {
  private:
    std::vector<double> input_data;
    std::vector<double> output_data;

  public:
    scenario1(size_t n, size_t seed);

  protected:
    std::string name() override;
    void run_omp() override;
    void run_thpool1() override;
    void run_thpool2() override;
};



class scenario2 : public scenario {
  private:
    using timepoint_t = std::chrono::high_resolution_clock::time_point;
    struct Timings {
      timepoint_t start;
      timepoint_t end;
    };

    size_t n;
    std::vector<std::vector<Timings>> timings;

  public:
    scenario2(size_t n_);
    void setup() override;
    void teardown() override;

  protected:
    std::string name() override;
    void run_omp() override;
    void run_thpool1() override;
    void run_thpool2() override;
};


#endif
