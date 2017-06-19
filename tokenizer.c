#include <stdio.h>
#include <stdlib.h>

#include "region.h"
#include "stringport.h"
#include "tokenizer.h"
#include "variables.h"
#include "reporting.h"

char tok_buf[TOK_MAX];

int
token_end(int chr)
{
	return chr == ' ' || chr == '\t' || chr == '\n' || chr == '#';
}

void
skip_spaces(string_port *stream)
{
	while (!port_eof(stream) && (port_peek(stream) == ' ' || port_peek(stream) == '\t'))
		port_getc(stream);
}

void
skip_newline(string_port *stream)
{
	skip_spaces(stream);
	if (port_peek(stream) == '\n')
		port_getc(stream);
	else
		return;
}

void
skip_comment(string_port *stream)
{
	while (!port_eof(stream) && port_getc(stream) != '\n');
}

int
token(string_port *stream)
{
	// TODO strings

	char *buf;
	int l;

	skip_spaces(stream);
	if (port_eof(stream) || port_peek(stream) == '\n')
		return -1;

	l = 0;
	buf = tok_buf;
	// WARNING: fpeek must be done before feof
	//          otherwise it may not succeed
	while (!token_end(port_peek(stream)) && !port_eof(stream)) {
		buf[l] = port_getc(stream);
		if (buf[l] == '\\') {
			buf[l] = port_getc(stream);
			port_peek(stream);
			if (port_eof(stream))
				reporterr("end of file when interpreting \\");
		}
		l++;
		if (l >= TOK_MAX-1)
			reporterr("Token too long!");
	}
	buf[l] = '\0';

	if (port_peek(stream) == '#')
		skip_comment(stream);

	if (!l)
		return -1;

	return l;
}

char **
read_tokens(region *r, string_port *f)
{
	char **tokens;
	int i, t;

	i = 0;

	tokens = region_malloc(r, sizeof(char*)*MAX_TOKS_PER_LINE);

	while ((t = token(f)) != -1) {
		if (!(tokens[i] = expand_variables(r, tok_buf, t)))
			return NULL;

		i++;
		if (i >= MAX_TOKS_PER_LINE)
			reporterr("Line too long!");
	}

	tokens[i] = NULL;

	return tokens;
}
