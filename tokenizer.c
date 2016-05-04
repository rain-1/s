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
	// WARNING: fpeek must be done before feof
	//          otherwise it may not succeed
	//
	while (!token_end(fpeek(stream)) && !feof(stream)) {
		buf[l] = fgetc(stream);
		l++;
		if (l >= TOK_MAX-1) {
			fprintf(stderr, "Token too long!");
			exit(-1);
		}
	}
	buf[l] = '\0';

	if (fpeek(stream) == '#')
		skip_comment(stream);

	if (!l)
		return -1;

	return l;
}
