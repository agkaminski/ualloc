/* Ualloc simple memory allocator
 * urealloc
 * Copyright: Aleksander Kaminski, 2018, 2024
 * See LICENSE.md
 */

#include "ualloc_priv.h"

void *urealloc(void *ptr, size_t size)
{
	header_t *curr, *spawn;
	unsigned char *buff;
	size_t t;

	if (!size) {
		ufree(ptr);
		return NULL;
	}

	size = ALIGN(size);

	if (ptr == NULL)
		return umalloc(size);

	curr = (header_t *)((unsigned char *)ptr - sizeof(header_t));

	if (SIZE(curr->size) >= size) {
		if (SIZE(curr->size) > size + sizeof(header_t) + ANTIFRAG) {
			spawn = (void *)(curr->payload + size);
			spawn->next = curr->next;
			spawn->size = SIZE(curr->size) - size - sizeof(header_t);
			curr->size = size | FLAG_MASK;
			curr->next = spawn;

			if (spawn->next != NULL && !FLAG(spawn->next->size)) {
				spawn->size = SIZE(spawn->size) + SIZE(spawn->next->size) + sizeof(header_t);
				spawn->next = spawn->next->next;
			}

			if (spawn < ualloc_hint)
				ualloc_hint = spawn;
		}

		return ptr;
	}

	if (curr->next != NULL && !FLAG(curr->next->size) &&
			(t = SIZE(curr->size) + SIZE(curr->next->size) + sizeof(header_t)) >= size) {

		if (curr->next == ualloc_hint)
			ualloc_hint = curr->next->next;

		curr->size = t | FLAG_MASK;
		curr->next = curr->next->next;

		if (t > size + sizeof(header_t) + ANTIFRAG) {
			curr->size = size | FLAG_MASK;
			spawn = (void *)(curr->payload + size);
			spawn->next = curr->next;
			spawn->size = t - size - sizeof(header_t);

			if (curr->next == ualloc_hint)
				ualloc_hint = spawn;

			curr->next = spawn;
		}

		return (void *)curr->payload;
	}

	if ((buff = umalloc(size)) != NULL) {
		memcpy(buff, ptr, SIZE(curr->size));
		ufree(ptr);
	}

	return buff;
}
