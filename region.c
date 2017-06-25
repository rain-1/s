#include <stdio.h>
#include <stdlib.h>

#include "region.h"
#include "reporting.h"

void
region_create(region *r)
{
	r->len = 0;
	r->alloc_len = 2;
	r->pointers = malloc(sizeof(void*)*r->alloc_len);
}

void *
region_malloc(region *r, size_t size)
{
	if (r->len >= r->alloc_len) {
		r->alloc_len <<= 1;
		r->pointers = realloc(r->pointers, sizeof(void*)*r->alloc_len);
	}

	return r->pointers[r->len++] = malloc(size);
}

void *
region_realloc(region *r, void *v, size_t size)
{
	int i;

	for (i = 0; i < r->len; i++)
		if (r->pointers[i] == v)
			return r->pointers[i] = realloc(r->pointers[i], size);

	reporterr("unable to realloc region [%p]", v);
}

void
region_free(region *r)
{
	int i;

	for (i = 0; i < r->len; i++)
		free(r->pointers[i]);

	free(r->pointers);

	r->pointers = NULL;
}
