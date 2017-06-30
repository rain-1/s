/* See LICENSE file for copyright and license details. */
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "reporting.h"

void *
ecalloc(size_t nmemb, size_t size)
{
	void *p;

	if (!(p = calloc(nmemb, size)))
		reporterr("calloc: out of memory");

	return p;
}

void *
emalloc(size_t size)
{
	void *p;

	if (!(p = malloc(size)))
		reporterr("malloc: out of memory");

	return p;
}

void *
erealloc(void *p, size_t size)
{
	if (!(p = realloc(p, size)))
		reporterr("realloc: out of memory");

	return p;
}

char *
estrdup(char *s)
{
	if (!(s = strdup(s)))
		reporterr("strdup: out of memory");

	return s;
}

void
efree(void *p)
{
	if (p)
		free(p);
}
