#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"

char tok_buf[TOK_MAX];

// from stackoverflow.com/questions/2082743/c-equivalent-to-fstreams-peek
int fpeek(FILE *stream)
{
	int c;

	c = fgetc(stream);
	ungetc(c, stream);

	return c;
}

void skip_spaces(FILE *stream)
{
	while (!feof(stream) && fpeek(stream) == ' ')
		fgetc(stream);
}

int token(FILE *stream)
{
	char *buf;
	int l;
	
	skip_spaces(stream);

	l = 0;
	buf = tok_buf;
	// TODO verify this is well bounded
	//
	// WARNING: fpeek must be done before feof
	//          otherwise it may not succeed
	//
	while (l < TOK_MAX-1 && fpeek(stream) != ' ' && !feof(stream)) {
		printf("<%c/%d/%d>\n", fpeek(stream), fpeek(stream), !feof(stream));
		buf[l] = fgetc(stream);
		l++;
	}
	buf[l] = '\0';

	return l;
}
