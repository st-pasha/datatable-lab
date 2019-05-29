#ifndef SCENARIO_h
#define SCENARIO_h


class scenario {
  public:
    virtual ~scenario();

    double time_omp();
    double time_threadpool();

  protected:
    virtual void run_omp() = 0;
    virtual void run_threadpool() = 0;
};



#endif
