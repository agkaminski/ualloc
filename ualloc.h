#ifndef _UALLOC_H_
#define _UALLOC_H_

#ifdef UALLOC_NO_LIBC
typedef unsigned int size_t;

static inline void *memcpy(void *dest, void *src, size_t n)
{
	while (n-- > 0)
		((unsigned char *)dest)[n] = ((unsigned char *)src)[n];
	
	return dest;
}

static inline void *memset(void *dest, int p, size_t n)
{
	while (n-- > 0)
		((unsigned char *)dest)[n] = (unsigned char)p;
	
	return dest;
}
#else
#include <stddef.h>
#include <string.h>
#endif

extern void *umalloc(size_t size);

extern void *ucalloc(size_t size);

extern void *urealloc(void *ptr, size_t size);

extern void ufree(void *ptr);

extern void ualloc_init(void *buff, size_t size);

#endif
