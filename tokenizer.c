#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"
#include "variables.h"

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
}

void skip_comment(FILE *stream)
{
	while (!feof(stream) && fgetc(stream) != '\n');
}

int token(FILE *stream)
{
	// TODO strings

	char *buf;
	int l;
	
	skip_spaces(stream);
	if (feof(stream) || fpeek(stream) == '\n')
		return -1;

	l = 0;
	buf = tok_buf;
	// WARNING: fpeek must be done before feof
	//          otherwise it may not succeed
	while (!token_end(fpeek(stream)) && !feof(stream)) {
		buf[l] = fgetc(stream); // TODO: backslash
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

char** read_tokens(FILE *f)
{
	char **tokens;
	int i, t;

	i = 0;

	tokens = malloc(sizeof(char*)*MAX_TOKS_PER_LINE);

	while ((t = token(f)) != -1) {
		tokens[i] = expand_variables(tok_buf, t);

		i++;
		if (i >= MAX_TOKS_PER_LINE) {
			fprintf(stderr, "Line too long!\n");
			exit(-1);
		}
	}

	tokens[i] = NULL;

	return tokens;
}
