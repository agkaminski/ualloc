#ifndef _UALLOC_H_
#define _UALLOC_H_

#ifdef UALLOC_NO_LIBC
typedef unsigned int size_t;
#else
#include <stddef.h>
#endif

extern void *umalloc(size_t size);

extern void *ucalloc(size_t num, size_t size);

extern void *urealloc(void *ptr, size_t size);

extern void free(void *ptr);

#endif
