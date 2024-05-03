/* Ualloc simple memory allocator
 * Copyright: Aleksander Kaminski, 2018, 2024
 * See LICENSE.md
 */


#ifndef _UALLOC_H_
#define _UALLOC_H_

#include <stddef.h>
#include <string.h>

extern void *umalloc(size_t size);

extern void *ucalloc(size_t size);

extern void *urealloc(void *ptr, size_t size);

extern void ufree(void *ptr);

extern void ualloc_init(void *buff, size_t size);

#endif
