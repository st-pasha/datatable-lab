
CC ?= gcc
INCLUDES = -I.
LIBRARIES =
CCFLAGS += -std=gnu++11 -stdlib=libc++ -O3 -fopenmp
LDFLAGS += -fopenmp

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
  CC = /usr/local/opt/llvm/bin/clang++
  INCLUDES += -I/usr/local/opt/llvm/include/c++/v1
  LIBRARIES += -L/usr/local/opt/llvm/lib
endif


clean:
	rm -f *.o
	rm -f ./parallel

build: main.o scenario.o
	$(CC) $(LDFLAGS) $(LIBRARIES) -o parallel $+

main.o: main.cc
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

scenario.o: scenario.cc scenario.h
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<
