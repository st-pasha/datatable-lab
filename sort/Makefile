
#-------------------------------------------------------------------------------

CC ?= gcc
INCLUDES = -I.
LIBRARIES =
CCFLAGS += -std=gnu++17 -stdlib=libc++

ifeq ($(DEBUG),)
	CCFLAGS += -O3
else
	CCFLAGS += -O0 -ggdb -DDEBUG
endif

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
  CC = /usr/local/opt/llvm/bin/clang++
  INCLUDES += -I/usr/local/opt/llvm/include/c++/v1
  LIBRARIES += -L/usr/local/opt/llvm/lib
endif



#-------------------------------------------------------------------------------

build: sort

main.o: main.cc
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

insert_sort.o: insert_sort.cc
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

merge_sort.o: merge_sort.cc
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

radix_sort.o: radix_sort.cc
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

sort: insert_sort.o merge_sort.o radix_sort.o main.o
	$(CC) $(LDFLAGS) -o $@ $+ $(LIBRARIES)

clean:
	rm -f *.o sort

debug:
	DEBUG=1 \
	$(MAKE) build

