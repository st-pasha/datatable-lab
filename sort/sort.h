#ifndef MICROBENCH_SORT_H
#define MICROBENCH_SORT_H
#include <new>
#include <stack>
#include <stdint.h>

template <typename T>
struct xoitem {
  T x;
  int o;

  xoitem() {}
  xoitem(T x_, int o_) : x(x_), o(o_) {}
  xoitem(const xoitem& other) : x(other.x), o(other.o) {}
  xoitem& operator=(const xoitem& other) {
    x = other.x;
    o = other.o;
    return *this;
  }
};


using sortfn_t = void (*)(void *x, int *o, int N, int K);
using sortfn2_t = void (*)(void* xo, int N, int K);

template <typename T>
void insert_sort0(T* x, int* o, int N, int);

template <typename T>
void insert_sort0_xo(xoitem<T>* xo, int N, int);

template <typename T>
void insert_sort2(T* x, int* o, int N, int);

template <typename T>
void insert_sort3(T* x, int* o, int N, int);

template <typename T>
void std_sort(xoitem<T>* xo, int n, int);

template <typename T, bool masked = false>
void count_sort0(T* x, int* o, int n, int K);

template <typename T>
void radix_sort1(T* x, int* o, int n, int K);

template <typename T>
void radix_sort3(T* x, int* o, int n, int K);

template <typename T, int P>
void merge_sort0(T* x, int* o, int N, int K);

template <typename T>
void mergesort0_impl(T* x, int* o, int n, T* t, int* u, int P);

void mergesort1(int* x, int* o, int n, int K);

void timsort(int* x, int* o, int n, int K);



struct omem {
  void* ptr;
  size_t n;
  std::stack<void*> ptr_stack;
  std::stack<size_t> n_stack;

  omem() : ptr(nullptr), n(0) {}
  ~omem() {
    while (!ptr_stack.empty()) pop();
    free(ptr);
  }

  size_t size() const {
    return n;
  }

  template <typename T>
  T* get() const {
    return (T*)ptr;
  }

  void ensure_size(size_t sz) {
    if (n > sz) return;
    n = sz;
    ptr = realloc(ptr, sz);
    if (ptr == nullptr) throw std::bad_alloc();
  }

  void push(void* x, size_t size) {
    ptr_stack.push(ptr);
    n_stack.push(n);
    ptr = x;
    n = size;
  }

  void pop() {
    ptr = ptr_stack.top();
    n = n_stack.top();
    ptr_stack.pop();
    n_stack.pop();
  }
};


extern int tmp0;
extern omem tmp1;
extern omem tmp2;
extern omem tmp3;


#endif
