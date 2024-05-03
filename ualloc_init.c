/* Ualloc simple memory allocator
 * ualloc_init
 * Copyright: Aleksander Kaminski, 2018, 2024
 * See LICENSE.md
 */

#include "ualloc_priv.h"

header_t *ualloc_heap = NULL;
header_t *ualloc_hint = NULL;

void ualloc_init(void *buff, size_t size)
{
	ualloc_heap = buff;
	ualloc_heap->size = (size - sizeof(header_t)) & ~FLAG_MASK;
	ualloc_heap->next = NULL;
	ualloc_hint = ualloc_heap;
}
