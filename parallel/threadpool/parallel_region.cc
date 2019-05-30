//------------------------------------------------------------------------------
// Copyright 2019 H2O.ai
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//------------------------------------------------------------------------------
#include "threadpool/api.h"
#include "threadpool/thread_pool.h"
#include "threadpool/thread_scheduler.h"
#include "threadpool/thread_team.h"
#include "threadpool/macros.h"          // cache_aligned
namespace dt {


//------------------------------------------------------------------------------
// once_scheduler
//------------------------------------------------------------------------------

// Implementation class for `dt::parallel_region()` function.
class once_scheduler : public thread_scheduler {
  private:
    struct simple_task : public thread_task {
      function<void()> f;
      simple_task(function<void()> f_) : f(f_) {}
      void execute(thread_worker*) override { f(); }
    };

    std::vector<cache_aligned<size_t>> done;
    simple_task task;

  public:
    once_scheduler(size_t nthreads, function<void()> fn);
    thread_task* get_next_task(size_t thread_index) override;
};

once_scheduler::once_scheduler(size_t nth, function<void()> fn)
  : done(nth, 0),
    task(fn) {}

thread_task* once_scheduler::get_next_task(size_t i) {
  if (i >= done.size() || done[i].v) {
    return nullptr;
  }
  done[i].v = 1;
  return &task;
}




//------------------------------------------------------------------------------
// parallel_region
//------------------------------------------------------------------------------

void parallel_region(function<void()> fn) {
  parallel_region(0, fn);
}

void parallel_region(size_t nthreads, function<void()> fn) {
  thread_pool* thpool = thread_pool::get_instance();
  assert(thpool->in_master_thread());
  size_t nthreads0 = thpool->size();
  if (nthreads > nthreads0 || nthreads == 0) nthreads = nthreads0;
  thread_team tt(nthreads, thpool);

  once_scheduler sch(nthreads, fn);
  thpool->execute_job(&sch);
}




}  // namespace dt
