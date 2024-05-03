/* Ualloc simple memory allocator
 * umalloc
 * Copyright: Aleksander Kaminski, 2018, 2024
 * See LICENSE.md
 */

#include "ualloc_priv.h"

void *umalloc(size_t size)
{
	header_t *curr, *spawn;

	if (!size || ualloc_heap == NULL)
		return NULL;

	size = align(size);

	if (ualloc_hint == NULL)
		ualloc_hint = ualloc_heap;

	for (curr = ualloc_hint; curr != NULL; curr = curr->next) {
		if (!FLAG(curr->size) && SIZE(curr->size) >= size) {
			if (SIZE(curr->size) >= size + sizeof(header_t) + ANTIFRAG) {
				spawn = (void *)(curr->payload + size);
				spawn->next = curr->next;
				spawn->size = (SIZE(curr->size) - size - sizeof(header_t)) & ~FLAG_MASK;

				curr->size = size;
				curr->next = spawn;
			}

			curr->size |= FLAG_MASK;

			if (curr == ualloc_hint)
				ualloc_hint = curr->next;

			return (void *)curr->payload;
		}
	}

	return NULL;
}
