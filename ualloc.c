#include "ualloc.h"

#define FLAG_MASK  (1UL)
#define FLAG(size) !!((size) & FLAG_MASK)
#define SIZE(size) ((size) & ~FLAG_MASK)
#define ANTIFRAG   8

static unsigned char *heap;

typedef struct _header_t {
	size_t size;
	struct _header_t *next;
} header_t;

static inline size_t align(size_t size)
{
	return (size + ANTIFRAG - 1) & ~(ANTIFRAG - 1);
}

void *umalloc(size_t size)
{
	header_t *curr, *t;

	if (!size)
		return NULL;

	size = align(size);

	for (curr = (header_t *)heap; curr != NULL; curr = curr->next) {
		if (!FLAG(curr->size) && SIZE(curr->size) >= size) {
			if (SIZE(curr->size) >= size + sizeof(header_t) + ANTIFRAG) {
				t = (void *)((char *)curr + size + sizeof(header_t));
				t->next = curr->next;
				t->size = (SIZE(curr->size) - size - sizeof(header_t)) & ~FLAG_MASK;

				curr->size = size;
				curr->next = t;
			}

			curr->size |= FLAG_MASK;

			return (void *)((char *)curr + sizeof(header_t));
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
		if ((void *)((char *)curr + sizeof(header_t)) == ptr)
			break;
	}

	if (curr == NULL)
		return;

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
}

void *urealloc(void *ptr, size_t size)
{
	header_t *curr, *spawn;
	unsigned char *buff;
	size_t t;

	if (!size) {
		ufree(ptr);
		return NULL;
	}

	size = align(size);
	
	if (ptr == NULL)
		return umalloc(size);
	
	for (curr = (header_t *)heap; curr != NULL; curr = curr->next) {
		if ((void *)((char *)curr + sizeof(header_t)) == ptr)
			break;
	}

	if (curr == NULL)
		return umalloc(size);

	if (SIZE(curr->size) >= size) {
		if (SIZE(curr->size) > size + sizeof(header_t) + ANTIFRAG) {
			spawn = (void *)((char *)curr + size + sizeof(header_t));
			spawn->next = curr->next;
			spawn->size = SIZE(curr->size) - size - sizeof(header_t);
			curr->size = size | FLAG_MASK;
			curr->next = spawn;

			if (spawn->next != NULL && !FLAG(spawn->next->size)) {
				spawn->size = SIZE(spawn->size) + SIZE(spawn->next->size) + sizeof(header_t);
				spawn->next = spawn->next->next;
			}
		}

		return ptr;
	}

	if (curr->next != NULL && !FLAG(curr->next->size) &&
		(t = SIZE(curr->size) + SIZE(curr->next->size) + sizeof(header_t)) >= size) {
		curr->size = t | FLAG_MASK;
		curr->next = curr->next->next;

		if (t > size + sizeof(header_t) + ANTIFRAG) {
			curr->size = size | FLAG_MASK;
			spawn = (void *)((char *)curr + sizeof(header_t) + size);
			spawn->next = curr->next;
			spawn->size = t - size - sizeof(header_t);
			curr->next = spawn;
		}

		return (void *)((char *)curr + sizeof(header_t));
	}
	
	if ((buff = umalloc(size)) == NULL)
		return NULL;

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
