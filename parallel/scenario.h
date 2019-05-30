#ifndef SCENARIO_h
#define SCENARIO_h
#include <memory>
#include <string>
#include <vector>

enum Backend {
  OMP = 1,
  THP = 2
};


class scenario {
  public:
    virtual ~scenario();

    void benchmark(int backends = Backend::OMP | Backend::THP);

  protected:
    virtual std::string name() = 0;
    virtual void run_omp() = 0;
    virtual void run_threadpool() = 0;
};

using scenptr = std::unique_ptr<scenario>;



class scenario1 : public scenario {
  private:
    std::vector<double> input_data;
    std::vector<double> output_data;

  public:
    explicit scenario1(size_t n);

  protected:
    std::string name() override;
    void run_omp() override;
    void run_threadpool() override;
};


#endif
