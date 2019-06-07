#ifndef MICROBENCH_SORT_H
#define MICROBENCH_SORT_H
#include <stdint.h>

extern int tmp0;
extern int *tmp1;
extern int *tmp2;
extern int *tmp3;

typedef void (*sortfn_t)(void *x, int *o, int N, int K);

template <typename T>
void insert_sort0(T* x, int* o, int N, int);


void iinsert2(int *x, int *o, int N, int K);
void iinsert3(int *x, int *o, int N, int K);
void iinsert3_i1(uint8_t *x, int *o, int n, int K);
void mergesort0(int *x, int *o, int N, int K);
void mergesort1(int *x, int *o, int n, int K);
void timsort(int *x, int *o, int n, int K);
void radixsort0(int *x, int *o, int n, int K);
void radixsort0_i1(uint8_t *x, int *o, int n, int K);
void radixsort1(int *x, int *o, int n, int K);
void radixsort2(int *x, int *o, int n, int K);
void radixsort2_i1(uint8_t *x, int *o, int n, int K);
void radixsort3(int *x, int *o, int n, int K);
void bestsort(int *x, int *o, int N, int K);

extern template void insert_sort0(uint32_t*, int*, int, int);
extern template void insert_sort0(uint8_t*, int*, int, int);


#endif
