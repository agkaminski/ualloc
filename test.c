#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ualloc.h"

#define NELEMS(x) (sizeof(x) / sizeof(x[0]))

char heap[16768];

char *ptr[128];
size_t size[128];

int check(void)
{
	int i, j;

	for (i = 0; i < NELEMS(ptr); ++i) {
		if (ptr[i] != NULL) {
			for (j = 0; j < size[i]; ++j) {
				if (ptr[i][j] != (char)i) {
					printf("Error ptr[%d][%d] = %d", i, j, ptr[i][j]);
					return -1;
				}
			}
		}
	}

	return 0;
}

void heap_show(void *heap)
{
	struct
	{
		size_t size;
		void *next;
	} *header;

	for (header = heap; header != NULL; header = header->next)
		printf("\t%p: [%zu, %s]\n", header, header->size & 0xffffffff, header->size > 0xffffffff ? "USED" : "FREE");
}

int main(void)
{
	int i, pos, err = 0;

	ualloc_init(heap, sizeof(heap));
	srand(1);

	for (i = 0; i < NELEMS(ptr); ++i) {
		ptr[i] = NULL;
		size[i] = 0;
	}

	for (i = 0; i < 10000; ++i) {
		pos = rand() % NELEMS(ptr);

		if (ptr[pos] != NULL)
			ufree(ptr[pos]);

		size[pos] = rand() & 0x1ff;
		ptr[pos] = umalloc(size[pos]);

		printf("%d: Alocated %zu bytes, got %p\n", pos, size[pos], ptr[pos]);

		if (ptr[pos] == NULL)
			continue;

		memset(ptr[pos], pos, size[pos]);

		if ((err = check()) != 0)
			break;

		heap_show(heap);
	}

	printf("Special cases\n");
	printf("Free all\n");
	for (i = 0; i < NELEMS(ptr); ++i)
		ufree(ptr[i]);

	heap_show(heap);

	printf("Biggest alloc\n");

	ptr[0] = umalloc(sizeof(heap) - 16);

	heap_show(heap);

	ufree(ptr[0]);

	heap_show(heap);

	printf("Too big\n");

	ptr[0] = umalloc(sizeof(heap));

	heap_show(heap);

	return 0;
}
