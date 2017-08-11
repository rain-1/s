/* see LICENSE file for copyright and license details */
#include <stdio.h>
#include <stdlib.h>

#include "region.h"
#include "reporting.h"
#include "util.h"

void
region_create(region *r)
{
	r->len = 0;
	r->alloc_len = 2;
	r->pointers = emalloc(sizeof(void*)*r->alloc_len);
}

void *
region_malloc(region *r, size_t size)
{
	if (r->len >= r->alloc_len) {
		r->alloc_len <<= 1;
		r->pointers = erealloc(r->pointers, sizeof(void*)*r->alloc_len);
	}

	return r->pointers[r->len++] = emalloc(size);
}

void *
region_realloc(region *r, void *v, size_t size)
{
	size_t i;

	for (i = 0; i < r->len; i++)
		if (r->pointers[i] == v)
			return r->pointers[i] = erealloc(r->pointers[i], size);

	reporterr("unable to realloc region [%p]", v);
}

void
region_free(region *r)
{
	size_t i;

	for (i = 0; i < r->len; i++)
		efree(r->pointers[i]);

	efree(r->pointers);

	r->pointers = NULL;
}
