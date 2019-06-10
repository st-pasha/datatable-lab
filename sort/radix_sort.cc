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
  static int INSERT_THRESHOLDS[] = {0, 8, 8, 8, 8, 12, 16, 16, 20, 20,
                                    20, 20, 20, 20, 20, 20, 20};
  if (N <= INSERT_THRESHOLDS[K]) {
    insert_sort0<T>(x, o, N, K);
  } else {
    count_sort0<T>(x, o, N, K);
  }
}


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
    bestsort<TO>(nextx, nexto, nextn, shift);
  }
  tmp1 = (int*)xx;
  tmp2 = oo;
}


// Radix Sort that first partially sorts by `tmp0` MSB bits, and then sorts
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
template void radix_sort1(uint32_t*, int*, int, int);
template void radix_sort1(uint64_t*, int*, int, int);




//------------------------------------------------------------------------------
// Radix Sort 2
//------------------------------------------------------------------------------

// Radix Sort that first partially sorts by `tmp0` MSB bits, and then sorts
// the remaining numbers using the counting sort.
// Note that the driver script allocates `1 << K` ints for buffer `tmp3`.
// We use here only `1 << tmp0` for the histogram, and then `1 << (K - tmp0)`
// for the recursive calls.
template <typename T>
void radix_sort2(T* x, int* o, int n, int K)
{
  // printf("radixsort2(x=%p, o=%p, n=%d, K=%d [tmp0=%d, tmp1=%p, tmp2=%p, tmp3=%p])\n",
  //        x, o, n, K, tmp0, tmp1, tmp2, tmp3);
  int nradixbits = tmp0;
  T*   xx = (T*)tmp1;
  int* oo = tmp2;
  int* histogram = tmp3;

  int nradixes = 1 << nradixbits;
  int shift = K - nradixbits;
  int mask = (1 << shift) - 1;
  std::memset(histogram, 0, nradixes * sizeof(int));

  // Generate the histogram
  for (int i = 0; i < n; i++) {
    // assert((x[i] >> shift) < nradixes && (x[i]>>shift) >= 0);
    histogram[x[i] >> shift]++;
  }
  int cumsum = 0;
  for (int i = 0; i < nradixes; i++) {
    int h = histogram[i];
    histogram[i] = cumsum;
    cumsum += h;
  }

  // Sort the variables using the histogram
  for (int i = 0; i < n; i++) {
    // assert((x[i] >> shift) < nradixes && (x[i]>>shift) >= 0);
    int k = histogram[x[i] >> shift]++;
    xx[k] = x[i] & mask;
    oo[k] = o[i];
    // assert(xx[k] >= 0 && xx[k] < (1 << shift));
  }

  // Continue sorting the remainder
  tmp2 = o;
  tmp3 = histogram + nradixes;
  for (int i = 0; i < nradixes; i++) {
    int start = i? histogram[i - 1] : 0;
    int end = histogram[i];
    int nextn = end - start;
    if (nextn <= 1) continue;
    T*   nextx = xx + start;
    int* nexto = oo + start;
    if (nextn <= 6) {
      insert_sort0<T>(nextx, nexto, nextn, shift);
    } else {
      // This will also use (and modify) tmp1 and tmp2
      count_sort0<T>(nextx, nexto, nextn, shift);
    }
  }
  tmp2 = oo;
  tmp3 = histogram;

  memcpy(o, oo, n * sizeof(int));
  // printf("  done.\n");
}

template void radix_sort2(uint8_t*,  int*, int, int);
template void radix_sort2(uint32_t*, int*, int, int);
template void radix_sort2(uint64_t*, int*, int, int);




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
template void radix_sort3(uint32_t*, int*, int, int);
template void radix_sort3(uint64_t*, int*, int, int);
