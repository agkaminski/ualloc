#include "ualloc.h"

#define FLAG_MASK  (1UL)
#define FLAG(size) !!((size) & FLAG_MASK)
#define SIZE(size) ((size) & ~FLAG_MASK)
#define ANTIFRAG   8

#ifdef __CC65__
#define inline
#endif

typedef struct _header_t {
	size_t size;
	struct _header_t *next;
	unsigned char payload[];
} header_t;

static header_t *heap = NULL;
static header_t *hint = NULL;

static inline size_t align(size_t size)
{
	return (size + ANTIFRAG - 1) & ~(ANTIFRAG - 1);
}

void *umalloc(size_t size)
{
	header_t *curr, *spawn;

	if (!size || heap == NULL)
		return NULL;

	size = align(size);

	if (hint == NULL)
		hint = heap;

	for (curr = hint; curr != NULL; curr = curr->next) {
		if (!FLAG(curr->size) && SIZE(curr->size) >= size) {
			if (SIZE(curr->size) >= size + sizeof(header_t) + ANTIFRAG) {
				spawn = (void *)(curr->payload + size);
				spawn->next = curr->next;
				spawn->size = (SIZE(curr->size) - size - sizeof(header_t)) & ~FLAG_MASK;

				curr->size = size;
				curr->next = spawn;
			}

			curr->size |= FLAG_MASK;

			if (curr == hint)
				hint = curr->next;

			return (void *)curr->payload;
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

	if (heap == NULL)
		return;

	for (curr = (header_t *)heap; curr != NULL; prev = curr, curr = curr->next) {
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

			if (curr < hint)
				hint = curr;

			break;
		}
	}
}

void *urealloc(void *ptr, size_t size)
{
	header_t *curr, *spawn;
	unsigned char *buff;
	size_t t;

	if (heap == NULL)
		return NULL;

	if (!size) {
		ufree(ptr);
		return NULL;
	}

	size = align(size);

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

			if (spawn < hint)
				hint = spawn;
		}

		return ptr;
	}

	if (curr->next != NULL && !FLAG(curr->next->size) &&
			(t = SIZE(curr->size) + SIZE(curr->next->size) + sizeof(header_t)) >= size) {

		if (curr->next == hint)
			hint = curr->next->next;

		curr->size = t | FLAG_MASK;
		curr->next = curr->next->next;

		if (t > size + sizeof(header_t) + ANTIFRAG) {
			curr->size = size | FLAG_MASK;
			spawn = (void *)(curr->payload + size);
			spawn->next = curr->next;
			spawn->size = t - size - sizeof(header_t);

			if (curr->next == hint)
				hint = spawn;

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

void ualloc_init(void *buff, size_t size)
{
	heap = buff;
	heap->size = (size - sizeof(header_t)) & ~FLAG_MASK;
	heap->next = NULL;
	hint = heap;
}
