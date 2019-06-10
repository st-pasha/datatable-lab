#include <chrono>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>  // getopt
#include <unistd.h>  // getopt_long
#include <assert.h>
#include "sort.h"

int tmp0 = 0;
int *tmp1 = NULL;
int *tmp2 = NULL;
int *tmp3 = NULL;

template <int s> struct _elt {};
template <> struct _elt<8> { using t = uint64_t; };
template <> struct _elt<4> { using t = uint32_t; };
template <> struct _elt<2> { using t = uint16_t; };
template <> struct _elt<1> { using t = uint8_t; };
template <int s>
using element_t = typename _elt<s>::t;



// S: element size of x
// N: number of items in array x (i.e. number of items to be sorted)
// K: max number of significant bits in elements x, this cannot exceed S*8
// B:
//
template <int S, bool combined=false>
int test(const char* algoname, sortfn_t sortfn, int N, int K, int B, int T, int seed)
{
  using XT = element_t<S>;
  assert(K <= S*8);
  assert(sizeof(XT) == S);
  XT* x = nullptr, *wx = nullptr;
  int* o = nullptr, *wo = nullptr;
  xoitem<XT>* xo = nullptr, *wxo = nullptr;

  if (combined) {
    xo = new xoitem<XT>[N];
  } else {
    x = new XT[N];   // data to be sorted
    o = new int[N];  // ordering, sorted together with the data
  }

  size_t niters = 0;
  double* ts = new double[B]();
  double tsum = 0;
  for (int b = 0; b < B; b++) {
    //----- Prepare data array -------------------------
    srand(seed + b * 101);
    XT mask = static_cast<XT>((1 << K) - 1);
    XT* xx = (XT*) x;
    for (int i = 0; i < N; i++) {
      XT z = static_cast<XT>(rand() & mask);
      if constexpr(combined) {
        xo[i] = {z, i};
      } else {
        xx[i] = z;
        o[i]  = i;
      }
    }

    //----- Determine the number of iterations ---------
    bool done = (N >= 32768);
    niters = 1;
    while (true) {
      if constexpr(combined) {
        wxo = new xoitem<XT>[N * niters];
      } else {
        wx = new XT[N * niters];
        wo = new int[N * niters];
      }
      for (int i = 0; i < niters; i++) {
        if constexpr(combined) {
          memcpy(wxo + i * N, xo, N * sizeof(xoitem<XT>));
        } else {
          memcpy(wx + i * N, x, N * sizeof(XT));
          memcpy(wo + i * N, o, N * sizeof(int));
        }
      }
      if (done) break;
      auto t0 = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < niters; i++) {
        if constexpr(combined) {
          xoitem<XT>* xoxo = wxo + i * N;
          reinterpret_cast<sortfn2_t>(sortfn)(xoxo, N, K);
        } else {
          XT*  xx = wx + i * N;
          int* oo = wo + i * N;
          sortfn(xx, oo, N, K);
        }
      }
      auto t1 = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> delta = t1 - t0;
      if (delta > std::chrono::milliseconds(1)) {
        double time_per_iter = delta.count() / niters;
        niters = (int)(0.99 + T * 1e-3 / (time_per_iter * B));
        if (niters < 1) niters = 1;
        done = true;
      } else {
        niters *= 2;
      }
      delete[] wx;
      delete[] wo;
      delete[] wxo;
    }

    //----- Run the iterations -------------------------
    auto t0 = std::chrono::high_resolution_clock::now();
    if constexpr(combined) {
      for (int i = 0; i < niters; i++) {
        xoitem<XT>* xoxo = wxo + i * N;
        reinterpret_cast<sortfn2_t>(sortfn)(xoxo, N, K);
      }
    } else {
      for (int i = 0; i < niters; i++) {
        XT*  xx = wx + i * N;
        int* oo = wo + i * N;
        sortfn(xx, oo, N, K);
      }
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> delta = t1 - t0;
    ts[b] = delta.count() / niters;
    tsum += ts[b];
    if ((tsum * 1000 > T && b >= 2) || tsum * 1000 > T * 3) {
      B = b + 1;
      break;
    }
  }

  //----- Process time stats -----------------------------
  double tavg;
  if (B >= 10) {
    double min1, min2, max1, max2;
    if (ts[0] < ts[1]) {
      min1 = max2 = ts[0];
      min2 = max1 = ts[1];
    } else {
      min1 = max2 = ts[1];
      min2 = max1 = ts[0];
    }
    double sumt = 0;
    for (int b = 0; b < B; b++) {
      double t = ts[b];
      sumt += t;
      if (t < min1) {
        min2 = min1;
        min1 = t;
      } else if (t < min2) {
        min2 = t;
      }
      if (t > max1) {
        max2 = max1;
        max1 = t;
      } else if (t > max2) {
        max2 = t;
      }
    }
    tavg = (sumt - min1 - min2 - max1 - max2) / (B - 4);
  } else {
    double sumt = 0;
    for (int b = 0; b < B; b++) sumt += ts[b];
    tavg = sumt / B;
  }
  printf("[%s]  %.3f ns\n", algoname, tavg * 1e9);
  // printf("Freeing x=%p, o=%p, wx=%p, wo=%p\n", x, o, wx, wo);
  delete[] x;
  delete[] o;
  delete[] xo;
  delete[] wx;
  delete[] wo;
  delete[] wxo;
  delete[] ts;
  return 0;
}



struct config {
  std::vector<int> algos;
  int batches;
  int n;
  int k;
  int time;
  int intsize;

  config() {
    batches = 100;
    n = 64;
    k = 16;
    time = 1000;
    intsize = 4;
  }

  void parse(int argc, char** argv) {
    struct option longopts[] = {
      // See https://linux.die.net/man/3/getopt_long
      // {name, has_arg, flag, val}
      {"algo", 1, 0, 0},
      {"batches", 1, 0, 0},
      {"n", 1, 0, 0},
      {"k", 1, 0, 0},
      {"time", 1, 0, 0},
      {"intsize", 1, 0, 0},
      {nullptr, 0, nullptr, 0}  // sentinel
    };

    while (1) {
      int option_index;
      int ret = getopt_long(argc, argv, "", longopts, &option_index);
      if (ret == -1) break;
      if (ret == 0) {
        if (optarg) {
          if (option_index == 0) algos.push_back(atol(optarg));
          if (option_index == 1) batches = atol(optarg);
          if (option_index == 2) n = atol(optarg);
          if (option_index == 3) k = atol(optarg);
          if (option_index == 4) time = atol(optarg);
          if (option_index == 5) intsize = atol(optarg);
        }
      }
    }
    if (algos.empty()) algos.push_back(1);
  }

  void report() {
    printf("\nInput parameters:\n");
    printf("  batches = %d\n", batches);
    printf("  n       = %d\n", n);
    printf("  k       = %d\n", k);
    printf("  time    = %d\n", time);
    printf("  intsize = %d\n", intsize);
    printf("\n");
  }
};





int main(int argc, char** argv) {
  // A - which algo to run (1-11):
  // B - number of batches, i.e. how many different datasets to try. Default
  //     is 100.
  // K - number of significant bits, i.e. each dataset will be comprised of
  //     random integers in the range [0, 1<<K)
  // N - array size
  // T - how long (in ms) to run the test for each algo, approximately.
  config cfg;
  cfg.parse(argc, argv);
  int B = cfg.batches;
  int N = cfg.n;
  int K = cfg.k;
  int T = cfg.time;
  int S = cfg.intsize;
  int seed = 1234; //time(NULL);
  printf("Array size (N) = %d\n", N);
  printf("N sig bits (K) = %d\n", K);
  printf("N batches  (B) = %d\n", B);
  printf("Exec. time (T) = %d ms\n", T);
  printf("Elem. size (S) = %d\n", S);
  printf("\n");
  if (S != 1 && S != 2 && S != 4 && S == 8) {
    printf("Unsupported integer size\n");
    exit(0);
  }
  if (S * 8 < K) {
    printf("Number of bits %d cannot exceed integer size %d\n", K, S*8);
    exit(0);
  }

  char name[100];
  tmp1 = new int[2*N];
  tmp2 = new int[N];
  tmp3 = new int[1 << K];

  for (int A : cfg.algos) {
    switch (A) {
      case 1:
        if (N <= 1024) {
          if (S == 1) test<1>("1:insert0", (sortfn_t)insert_sort0<uint8_t>,  N, K, B, T, seed);
          if (S == 2) test<2>("2:insert0", (sortfn_t)insert_sort0<uint16_t>, N, K, B, T, seed);
          if (S == 4) test<4>("4:insert0", (sortfn_t)insert_sort0<uint32_t>, N, K, B, T, seed);
          if (S == 8) test<8>("8:insert0", (sortfn_t)insert_sort0<uint64_t>, N, K, B, T, seed);
        }
        break;

      case 2:
        if (N <= 1024) {
          if (S == 1) test<1>("1:insert2", (sortfn_t)insert_sort2<uint8_t>,  N, K, B, T, seed);
          if (S == 2) test<2>("2:insert2", (sortfn_t)insert_sort2<uint16_t>, N, K, B, T, seed);
          if (S == 4) test<4>("4:insert2", (sortfn_t)insert_sort2<uint32_t>, N, K, B, T, seed);
          if (S == 8) test<8>("8:insert2", (sortfn_t)insert_sort2<uint64_t>, N, K, B, T, seed);
        }
        break;

      case 3:
        if (N <= 1024) {
          if (S == 1) test<1>("1:insert3", (sortfn_t)insert_sort3<uint8_t>,  N, K, B, T, seed);
          if (S == 2) test<2>("2:insert3", (sortfn_t)insert_sort3<uint16_t>, N, K, B, T, seed);
          if (S == 4) test<4>("4:insert3", (sortfn_t)insert_sort3<uint32_t>, N, K, B, T, seed);
          if (S == 8) test<8>("8:insert3", (sortfn_t)insert_sort3<uint64_t>, N, K, B, T, seed);
        }
        break;

      case 4:
        test<4>("mergeTD", (sortfn_t)mergesort0, N, K, B, T, seed);
        break;

      case 5:
        if (N <= 1000000) {
          test<4>("mergeBU", (sortfn_t)mergesort1, N, K, B, T, seed);
        }
        break;

      case 6:
        test<4>("timsort", (sortfn_t)timsort, N, K, B, T, seed);
        break;

      case 7:
        if (S == 1) test<1, true>("1:stdsort", (sortfn_t)std_sort<uint8_t>,  N, K, B, T, seed);
        if (S == 2) test<2, true>("2:stdsort", (sortfn_t)std_sort<uint16_t>, N, K, B, T, seed);
        if (S == 4) test<4, true>("4:stdsort", (sortfn_t)std_sort<uint32_t>, N, K, B, T, seed);
        if (S == 8) test<8, true>("8:stdsort", (sortfn_t)std_sort<uint64_t>, N, K, B, T, seed);
        break;

      case 8:
        if (K <= 20) {
          sprintf(name, "%d:count-%d", S, K);
          if (S == 1) test<1>(name, (sortfn_t)count_sort0<uint8_t>, N, K, B, T, seed);
          if (S == 2) test<2>(name, (sortfn_t)count_sort0<uint16_t>, N, K, B, T, seed);
          if (S == 4) test<4>(name, (sortfn_t)count_sort0<uint32_t>, N, K, B, T, seed);
          if (S == 8) test<8>(name, (sortfn_t)count_sort0<uint64_t>, N, K, B, T, seed);
        }
        break;

      case 9: {
        int kstep = K <= 4? 1 : K <= 8? 2 : 4;
        kstep = 8;
        for (tmp0 = kstep; tmp0 < K; tmp0 += kstep) {
          if (tmp0 > 20) break;
          sprintf(name, "radix1-%d/b", tmp0);
          test<4>(name, (sortfn_t)radix_sort1<uint32_t>, N, K, B, T, seed);
          sprintf(name, "radix2-%d/%d", tmp0, K - tmp0);
          test<4>(name, (sortfn_t)radix_sort2<uint32_t>, N, K, B, T, seed);
          if (K - tmp0 <= 16) {
            sprintf(name, "radix3-%d/b", tmp0);
            test<4>(name, (sortfn_t)radix_sort3<uint32_t>, N, K, B, T, seed);
          }
        }
      }
      break;

      case 10: {
        int kstep = K <= 4? 1 : K <= 8? 2 : 4;
        for (tmp0 = kstep; tmp0 < K; tmp0 += kstep) {
          if (K - tmp0 > 20) continue;
          if (tmp0 > 20) continue;
          sprintf(name, "radix1-%d/m", tmp0);
          test<4>(name, (sortfn_t)radix_sort1<uint32_t>, N, K, B, T, seed);
          sprintf(name, "radix2-%d/%d", tmp0, K - tmp0);
          test<4>(name, (sortfn_t)radix_sort2<uint32_t>, N, K, B, T, seed);
          if (K - tmp0 <= 16) {
            sprintf(name, "radix3-%d/b", tmp0);
            test<4>(name, (sortfn_t)radix_sort3<uint32_t>, N, K, B, T, seed);
          }
        }
        if (K <= 20) {
          sprintf(name, "radix%d", K);
          test<4>(name, (sortfn_t)count_sort0<uint32_t>, N, K, B, T, seed);
        }
      }
      break;

      case 11:
        if (K <= 8) {
          int kstep = K <= 4? 1 : K <= 8? 2 : 4;
          for (tmp0 = kstep; tmp0 < K; tmp0 += kstep) {
            if (K - tmp0 > 20) continue;
            if (tmp0 > 20) continue;
            sprintf(name, "radix2-%d/o", tmp0);
            test<1>(name, (sortfn_t)radix_sort2<uint8_t>, N, K, B, T, seed);
          }
        }
        break;

      default:
        printf("A = %d is not supported\n", A);
    }
  }

  // printf("Freeing tmp1=%p, tmp2=%p, tmp3=%p\n", tmp1, tmp2, tmp3);
  free(tmp1);
  free(tmp2);
  free(tmp3);
  return 0;
}
