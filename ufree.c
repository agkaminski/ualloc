/* Ualloc simple memory allocator
 * ufree
 * Copyright: Aleksander Kaminski, 2018, 2024
 * See LICENSE.md
 */

#include "ualloc_priv.h"

void ufree(void *ptr)
{
	header_t *curr, *prev = NULL;

	if (ualloc_heap == NULL)
		return;

	for (curr = (header_t *)ualloc_heap; curr != NULL; prev = curr, curr = curr->next) {
		if ((void *)curr->payload == ptr) {
			if (prev != NULL && !FLAG(prev->size)) {
				prev->size = SIZE(prev->size) + SIZE(curr->size) + sizeof(header_t);
				prev->next = curr->next;
				curr = prev;
			}

			if (curr->next != NULL && !FLAG(curr->next->size)) {
				curr->size = SIZE(curr->size) + SIZE(curr->next->size) + sizeof(header_t);
				curr->next = curr->next->next;
			}

			curr->size &= ~FLAG_MASK;

			if (curr < ualloc_hint)
				ualloc_hint = curr;

			break;
		}
	}
}
