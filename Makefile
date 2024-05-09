CC = gcc
AR = ar
CROSS ?=
LIB = ualloc
TEST = test

CFLAGS = -O2 -Wall -Werror #-DUALLOC_NO_LIBC

all: test.o ualloc.a
	$(CROSS)$(CC) $(CFLAGS) $^ -o $(TEST)

test.o: test.c
	$(CROSS)$(CC) $(CFLAGS) -c $< -o $@

ualloc.a: ualloc_init.o umalloc.o ufree.o ucalloc.o urealloc.o ustat.o
	$(CROSS)$(AR) rcs $@ $^

%.o: %.c
	$(CROSS)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a $(LIB) $(TEST)
