#include <omp.h>
#include "scenario.h"



int main(int argc, char** argv) {
  // Parsing input parameters...

  //
  auto sc = scenptr(new scenario1(1000000));
  sc->benchmark();
}
