CC = gcc
AR = ar
CROSS ?= 
LIB = ualloc
TEST = test

CFLAGS = -O2 -Wall -Werror -DUALLOC_NO_LIBC -DUALLOC_BSS

$(TEST): test.o ualloc.a
	$(CROSS)$(CC) $^ -o $@

test.o: test.c
	$(CROSS)$(CC) -c $< -o $@

ualloc.a: ualloc.o
	$(CROSS)$(AR) rcs $@ $^

ualloc.o: ualloc.c
	$(CROSS)$(CC) -c -o $@ $<

clean:
	rm -f *.o *.a $(LIB) $(TEST)