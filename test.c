#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ualloc.h"

#define NELEMS(x) (sizeof(x) / sizeof(x[0]))

char *ptr[128];
size_t size[128];

int check(void)
{
	int i, j;

	for (i = 0; i < NELEMS(ptr); ++i) {
		if (ptr != NULL) {
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

int main(void)
{
	int i, pos, err = 0;

	ualloc_init(NULL, 0);
	srand(0);

	for (i = 0; i < NELEMS(ptr); ++i) {
		ptr[i] = NULL;
		size[i] = 0;
	}

	for (i = 0; i < 10000; ++i) {
		pos = i % NELEMS(ptr);

		if (ptr[pos] != NULL)
			ufree(ptr[pos]);

		size[pos] = rand() & 0x1ff;
		ptr[pos] = umalloc(size[pos]);

		printf("%d: Alocated %zu bytes, got %p\n", i, size[pos], ptr[pos]);

		if (ptr[pos] == NULL)
			continue;

		memset(ptr[pos], pos, size[pos]);

		if ((err = check()) != 0)
			break;
	}

	printf("Passed.\n");

	return 0;
}
