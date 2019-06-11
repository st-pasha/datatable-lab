//==============================================================================
// insert-sort functions
//==============================================================================
#include <string.h>
#include <assert.h>
#include "sort.h"


template <typename T>
void insert_sort0(T* x, int* o, int n, int) {
  for (int i = 1, j = 0; i < n; ++i) {
    assert(j == i - 1);
    T xi = x[i];
    if (xi < x[j]) {
      int oi = o[i];
      while (j >= 0 && xi < x[j]) {
        x[j+1] = x[j];
        o[j+1] = o[j];
        j--;
      }
      x[j+1] = xi;
      o[j+1] = oi;
    }
    j = i;
  }
}


template <typename T>
void insert_sort0_xo(xoitem<T>* xo, int n, int) {
  int i, j, k;
  for (i = 1, j = 0; i < n; ++i) {
    assert(j == i - 1);
    T xi = xo[i].x;
    if (xi < xo[j].x) {
      int oi = xo[i].o;
      k = i;
      while (k > 0 && xi < xo[j].x) {
        assert(k == j + 1);
        xo[k] = xo[j];
        k = j--;
      }
      xo[k] = {xi, oi};
    }
    j = i;
  }
}



// Uses temporary array `tmp1`, which must be at least `n` ints long.
template <typename T>
void insert_sort2(T* x, int* o, int n, int K) {
  assert(tmp1.size() >= n * sizeof(int));
  int* t = tmp1.get<int>();
  t[0] = 0;
  for (int i = 1; i < n; i++) {
    T xi = x[i];
    int j = i;
    while (j && xi < x[t[j - 1]]) {
      t[j] = t[j - 1];
      j--;
    }
    t[j] = i;
  }
  for (int i = 0; i < n; i++) {
    t[i] = o[t[i]];
  }
  memcpy(o, t, n * sizeof(int));
}


// Two-way insert sort (see Knuth Vol.3)
// Uses temporary array `tmp1`, which must be at least `2 * n` ints long.
template <typename T>
void insert_sort3(T* x, int* o, int n, int)
{
  assert(tmp1.size() >= 2 * n * sizeof(int));
  int* t = tmp1.get<int>();
  t[n] = 0;
  int r = n, l = n, i, j, k;
  T xr, xl, xi;
  xr = xl = x[0];
  for (i = 1; i < n; i++) {
    xi = x[i];
    if (xi >= xr) {
      t[++r] = i;
      xr = xi;
    } else if (xi < xl) {
      t[--l] = i;
      xl = xi;
    } else {
      // Compute `j` such that `xi` has to be inserted between elements
      // `j` and `j-1`, i.e. such that `x[t[j-1]] <= xi < x[t[j]]`.
      if (xi < x[t[n]]) {
        j = n - 1;
        while (xi < x[t[j]]) j--;
        j++;
      } else {
        j = n + 1;
        while (xi >= x[t[j]]) j++;
      }
      assert(x[t[j - 1]] <= xi && xi < x[t[j]]);
      int rshift = r - j + 1;
      int lshift = j - l;
      if (rshift <= lshift) {
        // shift elements [j .. r] upwards by 1
        for (k = r; k >= j; k--) {
          t[k + 1] = t[k];
        }
        r++;
        t[j] = i;
      } else {
        // shift elements [l .. j-1] downwards by 1
        for (k = l; k < j; k++) {
          t[k - 1] = t[k];
        }
        l--;
        t[j-1] = i;
      }
    }
  }
  assert(r - l + 1 == n);
  for (i = l; i <= r; i++) {
    t[i] = o[t[i]];
  }
  memcpy(o, t + l, n * sizeof(int));
}


template void insert_sort0(uint8_t*,  int*, int, int);
template void insert_sort0(uint16_t*, int*, int, int);
template void insert_sort0(uint32_t*, int*, int, int);
template void insert_sort0(uint64_t*, int*, int, int);

template void insert_sort2(uint8_t*,  int*, int, int);
template void insert_sort2(uint16_t*, int*, int, int);
template void insert_sort2(uint32_t*, int*, int, int);
template void insert_sort2(uint64_t*, int*, int, int);

template void insert_sort3(uint8_t*,  int*, int, int);
template void insert_sort3(uint16_t*, int*, int, int);
template void insert_sort3(uint32_t*, int*, int, int);
template void insert_sort3(uint64_t*, int*, int, int);

template void insert_sort0_xo(xoitem<uint32_t>*, int, int);
template void insert_sort0_xo(xoitem<uint8_t>*, int, int);
