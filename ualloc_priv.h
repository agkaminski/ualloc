/* Ualloc simple memory allocator
 * Private header
 * Copyright: Aleksander Kaminski, 2018, 2024
 * See LICENSE.md
 */

#ifndef UALLOC_PRIV_H_
#define UALOOC_PRIV_H_

#include "ualloc.h"

#define FLAG_MASK  (1UL)
#define FLAG(size) !!((size) & FLAG_MASK)
#define SIZE(size) ((size) & ~FLAG_MASK)
#define ANTIFRAG   8

#define ALIGN(size) ((size) + ANTIFRAG - 1) & ~(ANTIFRAG - 1)

#ifdef __CC65__
#define inline
#endif

typedef struct _header_t {
	size_t size;
	struct _header_t *next;
	unsigned char payload[];
} header_t;

extern header_t *ualloc_heap;
extern header_t *ualloc_hint;

#endif
