#ifndef MICROBENCH_SORT_H
#define MICROBENCH_SORT_H
#include <stdint.h>

extern int tmp0;
extern int *tmp1;
extern int *tmp2;
extern int *tmp3;

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

template <typename T>
void count_sort0(T* x, int* o, int n, int K);

template <typename T>
void radix_sort1(T* x, int* o, int n, int K);

template <typename T>
void radix_sort2(T* x, int* o, int n, int K);

template <typename T>
void radix_sort3(T* x, int* o, int n, int K);

void mergesort0(int *x, int *o, int N, int K);
void mergesort1(int *x, int *o, int n, int K);
void timsort(int *x, int *o, int n, int K);



#endif
