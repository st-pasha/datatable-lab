
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


thpool1_objects = \
	thpool1/parallel_for_dynamic.o \
	thpool1/parallel_for_ordered.o \
	thpool1/parallel_for_static.o \
	thpool1/parallel_region.o \
	thpool1/thread_pool.o \
	thpool1/thread_scheduler.o \
	thpool1/thread_team.o \
	thpool1/thread_worker.o

thpool2_objects = \
	thpool2/parallel_for_dynamic.o \
	thpool2/parallel_for_ordered.o \
	thpool2/parallel_for_static.o \
	thpool2/parallel_region.o \
	thpool2/monitor_thread.o \
	thpool2/thread_pool.o \
	thpool2/thread_scheduler.o \
	thpool2/thread_team.o \
	thpool2/thread_worker.o

thpool3_objects = \
	thpool3/parallel_for_dynamic.o \
	thpool3/parallel_for_ordered.o \
	thpool3/parallel_for_static.o \
	thpool3/parallel_region.o \
	thpool3/monitor_thread.o \
	thpool3/thread_pool.o \
	thpool3/thread_scheduler.o \
	thpool3/thread_team.o \
	thpool3/thread_worker.o

all: build

clean:
	rm -f *.o
	rm -f threadpool/*.o
	rm -f thpool1/*.o
	rm -f thpool2/*.o
	rm -f thpool3/*.o
	rm -f ./parallel

debug:
	DEBUG=1 \
	$(MAKE) build

build: main.o scenario.o scenario1.o scenario2.o $(thpool1_objects) $(thpool2_objects) $(thpool3_objects)
	$(CC) $(LDFLAGS) $(LIBRARIES) -o parallel $+



main.o: main.cc
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

scenario.o: scenario.cc scenario.h
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

scenario1.o: scenario1.cc scenario.h
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

scenario2.o: scenario2.cc scenario.h
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<



thpool1_headers = \
	thpool1/api.h \
	thpool1/atomic.h \
	thpool1/shared_mutex.h \
	thpool1/thread_pool.h \
	thpool1/thread_scheduler.h \
	thpool1/thread_team.h \
	thpool1/thread_worker.h \
	utils/assert.h \
	utils/exceptions.h \
	utils/function.h \
	utils/macros.h

thpool1/parallel_for_dynamic.o: thpool1/parallel_for_dynamic.cc $(thpool1_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool1/parallel_for_ordered.o: thpool1/parallel_for_ordered.cc $(thpool1_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool1/parallel_for_static.o: thpool1/parallel_for_static.cc $(thpool1_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool1/parallel_region.o: thpool1/parallel_region.cc $(thpool1_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool1/thread_pool.o: thpool1/thread_pool.cc $(thpool1_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool1/thread_scheduler.o: thpool1/thread_scheduler.cc $(thpool1_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool1/thread_team.o: thpool1/thread_team.cc $(thpool1_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool1/thread_worker.o: thpool1/thread_worker.cc $(thpool1_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<




thpool2_headers = \
	thpool2/api.h \
	thpool2/atomic.h \
	thpool2/monitor_thread.h \
	thpool2/shared_mutex.h \
	thpool2/thread_pool.h \
	thpool2/thread_scheduler.h \
	thpool2/thread_team.h \
	thpool2/thread_worker.h \
	utils/assert.h \
	utils/exceptions.h \
	utils/function.h \
	utils/macros.h

thpool2/parallel_for_dynamic.o: thpool2/parallel_for_dynamic.cc $(thpool2_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool2/parallel_for_ordered.o: thpool2/parallel_for_ordered.cc $(thpool2_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool2/parallel_for_static.o: thpool2/parallel_for_static.cc $(thpool2_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool2/parallel_region.o: thpool2/parallel_region.cc $(thpool2_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool2/monitor_thread.o: thpool2/monitor_thread.cc $(thpool2_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool2/thread_pool.o: thpool2/thread_pool.cc $(thpool2_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool2/thread_scheduler.o: thpool2/thread_scheduler.cc $(thpool2_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool2/thread_team.o: thpool2/thread_team.cc $(thpool2_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool2/thread_worker.o: thpool2/thread_worker.cc $(thpool2_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<



thpool3_headers = \
	thpool3/api.h \
	thpool3/atomic.h \
	thpool3/monitor_thread.h \
	thpool3/shared_mutex.h \
	thpool3/thread_pool.h \
	thpool3/thread_scheduler.h \
	thpool3/thread_team.h \
	thpool3/thread_worker.h \
	utils/assert.h \
	utils/exceptions.h \
	utils/function.h \
	utils/macros.h

thpool3/parallel_for_dynamic.o: thpool3/parallel_for_dynamic.cc $(thpool3_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool3/parallel_for_ordered.o: thpool3/parallel_for_ordered.cc $(thpool3_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool3/parallel_for_static.o: thpool3/parallel_for_static.cc $(thpool3_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool3/parallel_region.o: thpool3/parallel_region.cc $(thpool3_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool3/monitor_thread.o: thpool3/monitor_thread.cc $(thpool3_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool3/thread_pool.o: thpool3/thread_pool.cc $(thpool3_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool3/thread_scheduler.o: thpool3/thread_scheduler.cc $(thpool3_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool3/thread_team.o: thpool3/thread_team.cc $(thpool3_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

thpool3/thread_worker.o: thpool3/thread_worker.cc $(thpool3_headers)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<
