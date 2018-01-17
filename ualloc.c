#include "ualloc.h"

#ifdef UALLOC_BSS
static unsigned char bssheap[UALLOC_BSS];
#endif

#define FLAG_MASK  (1 << (sizeof(size_t) << 3))
#define FLAG(size) !((size) & FLAG_MASK)
#define SIZE(size) ((size) & ~FLAG_MASK)

static unsigned char *heap;

typedef struct _header_t {
	size_t size;
	struct _header_t *next;
} header_t;

void *umalloc(size_t size)
{
	header_t *curr, *t;

	if (!FLAG(size))
		return NULL;

	curr = (header_t *)heap;

	for (curr = (header_t *)heap; curr != NULL; curr = curr->next) {
		if (FLAG(curr->size) && SIZE(curr->size) >= size + sizeof(header_t)) {
			t = curr + size + sizeof(header_t);
			t->next = curr->next;
			t->size = (SIZE(curr->size) - size - sizeof(header_t)) & ~FLAG_MASK;

			curr->size = size | FLAG_MASK;
			curr->next = t;

			return curr + sizeof(header_t);
		}
	}

	return NULL;
}

void *ucalloc(size_t num, size_t size)
{
	return NULL;
}

void *urealloc(void *ptr, size_t size)
{
	return NULL;
}

void free(void *ptr)
{
	header_t *curr, *prev = NULL;

	for (curr = (header_t *)heap; curr != NULL; prev = curr, curr = curr->next) {
		if (curr + sizeof(header_t) == ptr) {
			if (prev != NULL && FLAG(prev->size)) {
				prev->size = SIZE(prev->size) + SIZE(curr->size) + sizeof(header_t);
				prev->next = curr->next;
			}
			else if (curr->next != NULL && FLAG(curr->size)) {
				curr->size = SIZE(curr->size) + SIZE(curr->next->size) + sizeof(header_t);
				curr->next = curr->next->next;
			}
			else {
				curr->size &= ~FLAG_MASK;
			}
			break;
		}
	}
}

void ualloc_init(void *start, size_t size)
{
	size_t heapsz;
	header_t *header;

#ifdef UALLOC_BSS
	heap = bssheap;
	heapsz = UALLOC_BSS;
#else
	heap = start;
	heapsz = size;
#endif

	header = (header_t *)heap;
	header->size = heapsz - sizeof(header_t);
	header->flag = FFREE;
	header->next = NULL;
}
