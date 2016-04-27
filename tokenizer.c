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

int token_end(int chr)
{
	return chr == ' ' || chr == '\t' || chr == '\n' || chr == '#';
}

void skip_spaces(FILE *stream)
{
	while (!feof(stream) && (fpeek(stream) == ' ' || fpeek(stream) == '\t'))
		fgetc(stream);
}

void skip_newline(FILE *stream)
{
	skip_spaces(stream);
	if (fpeek(stream) == '\n')
		fgetc(stream);
	else
		return;
	skip_spaces(stream);
}

void skip_comment(FILE *stream)
{
	while (!feof(stream) && fgetc(stream) != '\n');
}

int token(FILE *stream)
{
	char *buf;
	int l;
	
	skip_spaces(stream);
	if (feof(stream) || fpeek(stream) == '\n')
		return -1;

	l = 0;
	buf = tok_buf;
	// TODO verify this is well bounded
	//
	// WARNING: fpeek must be done before feof
	//          otherwise it may not succeed
	//
	while (l < TOK_MAX-1 && !token_end(fpeek(stream)) && !feof(stream)) {
		buf[l] = fgetc(stream);
		l++;
	}
	buf[l] = '\0';

	if (fpeek(stream) == '#')
		skip_comment(stream);

	if (!l)
		return -1;

	return l;
}
