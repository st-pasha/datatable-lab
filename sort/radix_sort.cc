//==============================================================================
// Micro benchmark for radix sort function
//==============================================================================
#include <cstring>      // std::memset, std::memcpy
#include <type_traits>  // std::is_integral
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "sort.h"




//------------------------------------------------------------------------------
// Counting Sort 0
//------------------------------------------------------------------------------

// Counting sort (equivalent to radix sort using all K bits)
// tmp2 should have at least `n` ints.
// tmp3 should be at least `1 << K` ints long.
template <typename T>
void count_sort0(T* x, int* o, int n, int K)
{
  static_assert(std::is_integral<T>::value);
  static_assert(std::is_unsigned<T>::value);
  int* oo = tmp2;
  int* histogram = tmp3;

  int nradixes = 1 << K;
  std::memset(histogram, 0, nradixes * sizeof(int));

  // Generate the histogram
  for (int i = 0; i < n; i++) {
    histogram[x[i]]++;
  }
  int cumsum = 0;
  for (int i = 0; i < nradixes; i++) {
    int h = histogram[i];
    histogram[i] = cumsum;
    cumsum += h;
  }

  // Sort the variables using the histogram
  for (int i = 0; i < n; i++) {
    int k = histogram[x[i]]++;
    oo[k] = o[i];
  }
  std::memcpy(o, oo, n * sizeof(int));
}

template void count_sort0(uint8_t*,  int*, int, int);
template void count_sort0(uint16_t*, int*, int, int);
template void count_sort0(uint32_t*, int*, int, int);
template void count_sort0(uint64_t*, int*, int, int);




//------------------------------------------------------------------------------
// Radix Sort 1
//------------------------------------------------------------------------------

template <typename T>
static void bestsort(T* x, int* o, int N, int K)
{
  static int INSERT_THRESHOLDS[] = {0,
    12,  // k = 1
    12,  // k = 2
    12,  // k = 3
    12,  // k = 4
    15,  // k = 5
    19,  // k = 6
    19,  // k = 7
    20,  // k = 8
    20, 20, 20, 20, 20, 20, 20, 20};
  if (N <= INSERT_THRESHOLDS[K]) {
    insert_sort0<T>(x, o, N, K);
  } else {
    count_sort0<T>(x, o, N, K);
  }
}

template <typename T, int W>
static void bestsort0(T* x, int* o, int n, int K) {
  return n <= W ? insert_sort0<T>(x, o, n, K)
                : count_sort0<T>(x, o, n, K);
}

template <typename T, int W1, int W2>
static void bestsort1(T* x, int* o, int n, int K) {
  return n <= W1 ? insert_sort0<T>(x, o, n, K) :
         n <= W2 ? mergesort0_impl<T>(x, o, n, (T*)tmp1, tmp2, 20)
                 : count_sort0<T>(x, o, n, K);
}

using sortfn_u32_t = void(*)(uint32_t*, int*, int, int);
static sortfn_u32_t best_sorts_u32[] = {
  /* k =  0 */ nullptr,
  /* k =  1 */ bestsort0<uint32_t, 12>,
  /* k =  2 */ bestsort0<uint32_t, 12>,
  /* k =  3 */ bestsort0<uint32_t, 12>,
  /* k =  4 */ bestsort0<uint32_t, 12>,
  /* k =  5 */ bestsort0<uint32_t, 14>,
  /* k =  6 */ bestsort0<uint32_t, 19>,
  /* k =  7 */ bestsort0<uint32_t, 26>,
  /* k =  8 */ bestsort0<uint32_t, 28>,
  /* k =  9 */ bestsort1<uint32_t, 24, 48>,
  /* k = 10 */ bestsort1<uint32_t, 24, 72>,
  /* k = 11 */ bestsort1<uint32_t, 24, 72>, // tmp
  /* k = 12 */ bestsort1<uint32_t, 24, 72>, // tmp
  /* k = 13 */ bestsort1<uint32_t, 24, 72>, // tmp
  /* k = 14 */ bestsort1<uint32_t, 24, 72>, // tmp
  /* k = 15 */ bestsort1<uint32_t, 24, 72>, // tmp
  /* k = 16 */ bestsort1<uint32_t, 24, 72>, // tmp
};


template <typename TI, typename TO>
static void radix_recurse(TI* x, int* o, int* histogram, int n,
                          int nradixes, int shift)
{
  TO*  xx = (TO*)tmp1;
  int* oo = tmp2;
  int mask = nradixes - 1;

  for (int i = 0; i < n; i++) {
    int k = histogram[x[i] >> shift]++;
    xx[k] = (TO)(x[i] & mask);
    oo[k] = o[i];
  }

  // Continue sorting the remainder
  tmp1 = (int*)x;
  tmp2 = o;
  for (int i = 0; i < nradixes; i++) {
    int start = i? histogram[i - 1] : 0;
    int end = histogram[i];
    int nextn = end - start;
    if (nextn <= 1) continue;
    TO*  nextx = xx + start;
    int* nexto = oo + start;
    if constexpr(std::is_same<TO, uint32_t>::value) {
      best_sorts_u32[shift](nextx, nexto, nextn, shift);
    } else {
      bestsort<TO>(nextx, nexto, nextn, shift);
    }
  }
  tmp1 = (int*)xx;
  tmp2 = oo;
}


// Radix Sort that first partially sorts by `k = tmp0` MSB bits, and then sorts
// the remaining numbers using "best" sort.
// Uses:
//   tmp1 - array of the same size as x (i.e. n*sizeof(T))
//   tmp2 - array of the same size as o (i.e. n*sizeof(int))
//   tmp3 - array of size (1<<tmp0) * sizeof(int)
template <typename T>
void radix_sort1(T* x, int* o, int n, int K)
{
  // printf("radixsort1(x=%p, o=%p, n=%d, K=%d)\n", x, o, n, K);
  int nradixbits = tmp0;
  T*  xx = reinterpret_cast<T*>(tmp1);
  int* oo = tmp2;
  int* histogram = tmp3;

  int nradixes = 1 << nradixbits;
  int shift = K - nradixbits;
  int mask = nradixes - 1;
  // printf("  nradixes=%d, shift=%d, mask=%d\n", nradixes, shift, mask);
  std::memset(histogram, 0, nradixes * sizeof(int));

  // Generate the histogram
  // printf("  generate histogram...\n");
  for (int i = 0; i < n; i++) {
    histogram[x[i] >> shift]++;
  }
  int cumsum = 0;
  for (int i = 0; i < nradixes; i++) {
    int h = histogram[i];
    histogram[i] = cumsum;
    cumsum += h;
  }

  // Sort the variables using the histogram
  radix_recurse<T, T>(x, o, histogram, n, nradixes, shift);

  std::memcpy(o, oo, n * sizeof(int));
}

template void radix_sort1(uint8_t*,  int*, int, int);
template void radix_sort1(uint16_t*, int*, int, int);
template void radix_sort1(uint32_t*, int*, int, int);
template void radix_sort1(uint64_t*, int*, int, int);




//------------------------------------------------------------------------------
// Radix Sort 3
//------------------------------------------------------------------------------

// This is exactly like radixsort0, but stores output x array more compactly:
// either as uint8_t or uint16_t.
template <typename T>
void radix_sort3(T* x, int* o, int n, int K)
{
  int nradixbits = tmp0;
  int* oo = tmp2;
  int* histogram = tmp3;

  int nradixes = 1 << nradixbits;
  int shift = K - nradixbits;
  std::memset(histogram, 0, nradixes * sizeof(int));

  // Generate the histogram
  for (int i = 0; i < n; i++) {
    histogram[x[i] >> shift]++;
  }
  int cumsum = 0;
  for (int i = 0; i < nradixes; i++) {
    int h = histogram[i];
    histogram[i] = cumsum;
    cumsum += h;
  }

  tmp3 = histogram + nradixes;
  if (shift <= 8)       radix_recurse<T, uint8_t >(x, o, histogram, n, nradixes, shift);
  else if (shift <= 16) radix_recurse<T, uint16_t>(x, o, histogram, n, nradixes, shift);
  else if (shift <= 32) radix_recurse<T, uint32_t>(x, o, histogram, n, nradixes, shift);
  else                  radix_recurse<T, uint64_t>(x, o, histogram, n, nradixes, shift);
  tmp3 = histogram;

  memcpy(o, oo, n * sizeof(int));
}

template void radix_sort3(uint8_t*,  int*, int, int);
template void radix_sort3(uint16_t*, int*, int, int);
template void radix_sort3(uint32_t*, int*, int, int);
template void radix_sort3(uint64_t*, int*, int, int);
