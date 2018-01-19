CC = gcc
AR = ar
CROSS ?= 
LIB = ualloc
TEST = test

CFLAGS = -O0 -g -Wall -Werror -DUALLOC_BSS=16768 #-DUALLOC_NO_LIBC

all: test.o ualloc.a
	$(CROSS)$(CC) $(CFLAGS) $^ -o $(TEST)

test.o: test.c
	$(CROSS)$(CC) $(CFLAGS) -c $< -o $@

ualloc.a: ualloc.o
	$(CROSS)$(AR) rcs $@ $^

ualloc.o: ualloc.c
	$(CROSS)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a $(LIB) $(TEST)
