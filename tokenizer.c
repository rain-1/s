#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "region.h"
#include "stringport.h"
#include "tokenizer.h"
#include "variables.h"
#include "reporting.h"

/* TOK(c) adds a character c to the buffer, erroring if we went over the limit */
#define TOK(c)					\
	if (len >= TOK_MAX-1)			\
		reporterr("token too long");	\
	tok_buf[len++] = c

char tok_buf[TOK_MAX];

int
is_escape_char(int chr)
{
	return chr == '\\' || chr == 't' || chr == 'n' || chr == 'r' ||
	       chr == '"' || chr == '\'';
}

int
token_end(int chr)
{
	return chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r' ||
	       chr == '#';
}

void
skip_spaces(string_port *stream)
{
	while (!port_eof(stream) &&
	       (port_peek(stream) == ' ' || port_peek(stream) == '\t'))
		port_getc(stream);
}

void
skip_eol(string_port *stream)
{
	while (!port_eof(stream)) {
		if (port_getc(stream) == '\n')
			break;
	}
}

void
skip_spaces_and_comments(string_port *stream)
{
	while (!port_eof(stream)) {
		if (port_peek(stream) == ' ' || port_peek(stream) == '\t') {
			port_getc(stream);
		}
		else if (port_peek(stream) == '#') {
			skip_eol(stream);
		}
		else {
			break;
		}
	}
}

void
skip_newline(string_port *stream)
{
	skip_spaces(stream);

	if (port_peek(stream) == '\n')
		port_getc(stream);
}

/* returns -1 on failure, length of the token on success */
/* a word token cannot have length 0 but a string token can */
int
read_token(string_port *stream, int *out_should_expand)
{
	int len = 0;
	char c;

	char quote;
	int escape_char;

	*out_should_expand = 1;

	// this routine is used to read the next
	// token in a 'line' of tokens
	// therefore we need to exit if we hit a
	// newline or a comment
st_restart:
	skip_spaces(stream);
	if (port_eof(stream)
	    || port_peek(stream) == '\n'
	    || port_peek(stream) == '#')
		return -1;

	goto st_tok; /* parse using a state machine */

st_tok:
	c = port_getc(stream);

	if (c == '"' || c == '\'') {
		quote = c;
		escape_char = 0;

		if (c == '\'')
			*out_should_expand = 0;

		goto st_string;
	} else if (c == '~') {
		TOK('$');
		TOK('{');
		TOK('H');
		TOK('O');
		TOK('M');
		TOK('E');
		TOK('}');
		goto st_word_continue;
	} else if (c == '\\') {
		if (port_peek(stream) == '\n') {
			port_getc(stream);
			goto st_restart;
		}
		else
			goto st_word;
	} else {
		goto st_word;
	}

st_word:
	if (c == '\\') {
		if (port_eof(stream))
			return -1;
		
		c = port_getc(stream);
	}

	TOK(c);

st_word_continue:
	if (port_eof(stream) || token_end(port_peek(stream))) {
		if (len)
			goto st_accept;
		else
			return -1;
	}
	else {
		c = port_getc(stream);
		
		goto st_word;
	}
	
st_string:
	if (port_eof(stream))
		return -1;

	c = port_getc(stream);
	if (!escape_char && c == quote) {
		// check that the very next char
		// is not another string
		if (port_peek(stream) == '"' ||
		    port_peek(stream) == '\'') {
			reporterr("strings too close together");
		}
		
		goto st_accept;
	} else if (!escape_char && c == '\\') {
		escape_char = 1;
		goto st_string;
	} else if (escape_char && is_escape_char(c)) {
		escape_char = 0;
		switch(c) {
		case '\\':
			TOK('\\');
			break;
		case 't':
			TOK('\t');
			break;
		case 'n':
			TOK('\n');
			break;
		case 'r':
			TOK('\r');
			break;
		case '"':
			TOK('"');
			break;
		case '\'':
			TOK('\'');
			break;
		default:
			reporterr("impossible escape");
		}
		goto st_string;
	} else {
		if (escape_char)
			reporterr("escaped a non-escapable char");
		TOK(c);
		goto st_string;
	}

st_accept:
	tok_buf[len] = '\0';

	return len;
}

char **
read_tokens(region *r, string_port *stream)
{
	char **tokens;
	int i = 0;

	int len, should_expand;

	tokens = region_malloc(r, sizeof(char*)*MAX_TOKS_PER_LINE);
	
	skip_spaces_and_comments(stream);
	
	while ((len = read_token(stream, &should_expand)) != -1) {
		if (should_expand) {
			if (!(tokens[i] = expand_variables(r, tok_buf, len)))
				return NULL;
		} else {
			tokens[i] = region_malloc(r, len + 1);
			strncpy(tokens[i], tok_buf, len + 1);
		}

		if (++i >= MAX_TOKS_PER_LINE)
			reporterr("line too long");
	}

	tokens[i] = NULL;

	return tokens;
}
