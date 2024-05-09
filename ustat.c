/* Ualloc simple memory allocator
 * ustat
 * Copyright: Aleksander Kaminski 2024
 * See LICENSE.md
 */

#include "ualloc_priv.h"

void ustat(size_t *used, size_t *free)
{
	header_t *curr = ualloc_heap;
	size_t u = 0, f = 0, sz;

	while (curr != NULL) {
		sz = SIZE(curr->size);
		if (FLAG(curr->size)) {
			u += sz;
		}
		else {
			f += sz;
		}
		curr = curr->next;
	}

	if (used != NULL) {
		*used = u;
	}
	if (free != NULL) {
		*free = f;
	}
}
