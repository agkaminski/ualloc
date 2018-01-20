#include "ualloc.h"

#define FLAG_MASK  (1UL << ((sizeof(size_t) << 3) - 1))
#define FLAG(size) !!((size) & FLAG_MASK)
#define SIZE(size) ((size) & ~FLAG_MASK)
#define ANTIFRAG   8

static unsigned char *heap;

typedef struct _header_t {
	size_t size;
	struct _header_t *next;
} header_t;

void *umalloc(size_t size)
{
	header_t *curr, *t;

	if (FLAG(size) || !size)
		return NULL;

	if (size < ANTIFRAG)
		size = ANTIFRAG;

	for (curr = (header_t *)heap; curr != NULL; curr = curr->next) {
		if (!FLAG(curr->size)) {
			if (SIZE(curr->size) >= size + sizeof(header_t) + ANTIFRAG) {
				t = (void *)curr + size + sizeof(header_t);
				t->next = curr->next;
				t->size = (SIZE(curr->size) - size - sizeof(header_t)) & ~FLAG_MASK;

				curr->size = size | FLAG_MASK;
				curr->next = t;

				return (void *)curr + sizeof(header_t);
			}
			else if (SIZE(curr->size >= size)) {
				curr->size |= FLAG_MASK;

				return (void *)curr + sizeof(header_t);
			}
		}
	}

	return NULL;
}

void *ucalloc(size_t size)
{
	void *ptr = umalloc(size);
	
	if (ptr != NULL)
		memset(ptr, 0, size);
	
	return ptr;
}

void ufree(void *ptr)
{
	header_t *curr, *prev = NULL;

	for (curr = (header_t *)heap; curr != NULL; prev = curr, curr = curr->next) {
		if ((void *)curr + sizeof(header_t) == ptr) {
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

			break;
		}
	}
}

void *urealloc(void *ptr, size_t size)
{
	header_t *curr, *spawn;
	unsigned char *buff;
	size_t t;
	
	if (FLAG(size))
		return NULL;
	
	if (ptr == NULL)
		return umalloc(size);
	
	for (curr = (header_t *)heap; curr != NULL; curr = curr->next) {
		if ((void *)curr + sizeof(header_t) == ptr) {
			if (!FLAG(curr->next->size)) {
				if ((t = SIZE(curr->size) + SIZE(curr->next->size) + sizeof(header_t)) >= size) {
					curr->size = size | FLAG_MASK;
					curr->next = curr->next->next;
					
					if (t > size + sizeof(header_t) + ANTIFRAG) {
						spawn = (void *)curr + sizeof(header_t) + size;
						spawn->next = curr->next;
						spawn->size = t - size;
						curr->next = spawn;
					}
					else if (t != size) {
						curr->size = t | FLAG_MASK;
					}
					
					return (void *)curr + sizeof(header_t);
				}
				else {
					break;
				}
			}
			else {
				break;
			}
		}
	}
	
	if (curr == NULL)
		return umalloc(size);
	
	buff = umalloc(size);
	memcpy(buff, ptr, SIZE(curr->size));
	ufree(ptr);
	
	return buff;
}

void ualloc_init(void *buff, size_t size)
{
	header_t *header;

	heap = buff;
	header = (header_t *)buff;

	header->size = (size - sizeof(header_t)) & ~FLAG_MASK;
	header->next = NULL;
}
