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
#include <thread>      // std::thread::hardware_concurrency
#include <pthread.h>   // pthread_atfork
#include "thpool2/api.h"
#include "thpool2/thread_pool.h"
#include "thpool2/thread_team.h"
#include "thpool2/thread_worker.h"
#include "utils/assert.h"
namespace dt2 {


// Singleton instance of the thread_pool
thread_pool* thpool = new thread_pool;

static void _child_cleanup_after_fork() {
  // Replace the current thread pool instance with a new one, ensuring that all
  // schedulers and workers have new mutexes/condition variables.
  // The previous value of `thpool` is abandoned without deleting since that
  // memory is owned by the parent process.
  size_t n = thpool->size();
  thpool = new thread_pool;
  thpool->resize(n);
}



//------------------------------------------------------------------------------
// thread_pool
//------------------------------------------------------------------------------
static bool after_fork_handler_registered = false;

thread_pool::thread_pool()
  : num_threads_requested(0),
    current_team(nullptr)
{
  if (!after_fork_handler_registered) {
    pthread_atfork(nullptr, nullptr, _child_cleanup_after_fork);
    after_fork_handler_registered = true;
  }
}

// In the current implementation the thread_pool instance never gets deleted
// thread_pool::~thread_pool() {
//   resize(0);
// }


size_t thread_pool::size() const noexcept {
  return num_threads_requested;
}

void thread_pool::resize(size_t n) {
  num_threads_requested = n;
  // Adjust the actual thread count, but only if the threads were already
  // instantiated.
  if (!workers.empty()) {
    instantiate_threads();
  }
}

void thread_pool::instantiate_threads() {
  size_t n = num_threads_requested;
  if (workers.size() == n) return;
  if (workers.size() < n) {
    workers.reserve(n);
    for (size_t i = workers.size(); i < n; ++i) {
      try {
        auto worker = new thread_worker(i, &controller);
        workers.push_back(worker);
      } catch (...) {
        // If threads cannot be created (for example if user has requested
        // too many threads), then stop creating new workers and use as
        // many as we were able to create thus far.
        n = num_threads_requested = i;
      }
    }
    // Wait until all threads are properly alive & safely asleep
    controller.join();
  }
  else {
    thread_team tt(n, this);
    thread_shutdown_scheduler tss(n, &controller);
    execute_job(&tss);
    for (size_t i = n; i < workers.size(); ++i) {
      delete workers[i];
    }
    workers.resize(n);
  }
  xassert(workers.size() == num_threads_requested);
}


void thread_pool::execute_job(thread_scheduler* job) {
  xassert(current_team);
  if (workers.empty()) instantiate_threads();
  controller.awaken_and_run(job, workers.size());
  controller.join();
  // careful: workers.size() may not be equal to num_threads_requested during
  // shutdown
}


bool thread_pool::in_parallel_region() const noexcept {
  return (current_team != nullptr);
}

size_t thread_pool::n_threads_in_team() const noexcept {
  return current_team? current_team->size() : 0;
}


thread_team* thread_pool::get_team_unchecked() noexcept {
  return thpool->current_team;
}




//------------------------------------------------------------------------------
// Misc
//------------------------------------------------------------------------------

size_t num_threads_in_pool() {
  return thpool->size();
}

size_t num_threads_in_team() {
  return thpool->n_threads_in_team();
}

size_t num_threads_available() {
  auto tt = thpool->get_team_unchecked();
  return tt? tt->size() : thpool->size();
}

static thread_local size_t thread_index = 0;
size_t this_thread_index() {
  return thread_index;
}
void _set_thread_num(size_t i) {
  thread_index = i;
}


size_t get_hardware_concurrency() noexcept {
  unsigned int nth = std::thread::hardware_concurrency();
  if (!nth) nth = 1;
  return static_cast<size_t>(nth);
}



static int init_statically() {
  thpool->resize(get_hardware_concurrency());
  return 1;
}
static int _done = init_statically();


}  // namespace dt
