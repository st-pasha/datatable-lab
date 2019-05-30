
CC ?= gcc
INCLUDES = -I.
LIBRARIES =
CCFLAGS += -std=gnu++11 -stdlib=libc++ -fopenmp
LDFLAGS += -fopenmp

ifeq ($(DEBUG),)
	CCFLAGS += -O3
else
	CCFLAGS += -O0 -ggdb
endif

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
  CC = /usr/local/opt/llvm/bin/clang++
  INCLUDES += -I/usr/local/opt/llvm/include/c++/v1
  LIBRARIES += -L/usr/local/opt/llvm/lib
endif

threadpool_objects = \
	threadpool/parallel_for_dynamic.o \
	threadpool/parallel_for_ordered.o \
	threadpool/parallel_for_static.o \
	threadpool/parallel_region.o \
	threadpool/thread_pool.o \
	threadpool/thread_scheduler.o \
	threadpool/thread_team.o \
	threadpool/thread_worker.o

all: build

clean:
	rm -f *.o
	rm -f threadpool/*.o
	rm -f ./parallel

debug:
	DEBUG=1 \
	$(MAKE) build

build: main.o scenario.o scenario1.o $(threadpool_objects)
	$(CC) $(LDFLAGS) $(LIBRARIES) -o parallel $+



main.o: main.cc
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

scenario.o: scenario.cc scenario.h
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

scenario1.o: scenario1.cc scenario.h
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<


threadpool_headers = \
	threadpool/api.h \
	threadpool/atomic.h \
	threadpool/function.h \
	threadpool/macros.h \
	threadpool/shared_mutex.h \
	threadpool/thread_pool.h \
	threadpool/thread_scheduler.h \
	threadpool/thread_team.h \
	threadpool/thread_worker.h


threadpool/parallel_for_dynamic.o: threadpool/parallel_for_dynamic.cc $(threadpool_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

threadpool/parallel_for_ordered.o: threadpool/parallel_for_ordered.cc $(threadpool_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

threadpool/parallel_for_static.o: threadpool/parallel_for_static.cc $(threadpool_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

threadpool/parallel_region.o: threadpool/parallel_region.cc $(threadpool_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

threadpool/thread_pool.o: threadpool/thread_pool.cc $(threadpool_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

threadpool/thread_scheduler.o: threadpool/thread_scheduler.cc $(threadpool_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

threadpool/thread_team.o: threadpool/thread_team.cc $(threadpool_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

threadpool/thread_worker.o: threadpool/thread_worker.cc $(threadpool_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<


