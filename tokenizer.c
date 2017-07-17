/* see LICENSE file for copyright and license details */
/* split stringport input into a string array for each token */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "region.h"
#include "stringport.h"
#include "tokenizer.h"
#include "variables.h"
#include "reporting.h"
#include "parser.h"
#include "interpreter.h"
#include "util.h"

enum {
	EXPAND_DEFAULT,
	EXPAND_NONE,
	EXPAND_EVAL,
};

char escs[][2] = {
	{ '\\', '\\' },
	{ 't',  '\t' },
	{ 'n',  '\n' },
	{ 'r',  '\r' },
	{ '"',  '"'  },
	{ '\'', '\'' },
	{ '`',  '`'  }
};

static int
is_esc(int c)
{
	int i;
	for (i = 0; i < LEN(escs); i++)
		if (escs[i][0] == c)
			return 1;
	return 0;
}

static void
skip_spaces(string_port *stream)
{
	while (!port_eof(stream) &&
	       (port_peek(stream) == ' ' || port_peek(stream) == '\t'))
		port_getc(stream);
}

static void
skip_spaces_and_comments(string_port *stream)
{
	while (!port_eof(stream))
		if (port_peek(stream) == ' ' || port_peek(stream) == '\t')
			port_getc(stream);
		else if (port_peek(stream) == '#')
			while (!port_eof(stream) && port_getc(stream) != '\n') ;
		else break;
}

void
skip_newline(string_port *stream)
{
	skip_spaces(stream);

	if (port_peek(stream) == '\n')
		port_getc(stream);
}

/* returns -1 on failure, length of the token on success
 * a word token cannot have length 0 but a string token can */
static int
read_token(char *tok_buf, string_port *stream, int *out_should_expand)
{
	int len = 0, escape_char, i;
	char c, quote;

/* TOK(c) adds a character c to the buffer, erroring if it went over the limit */
#define TOK(c)					\
	if (len > TOK_MAX)			\
		reporterr("token too long");	\
	tok_buf[len++] = c

	*out_should_expand = EXPAND_DEFAULT;

/* this routine is used to read the next token in a 'line' of tokens therefore
 * we need to exit if we hit a newline or a comment */
st_restart:
	skip_spaces(stream);
	if (port_eof(stream) ||
	    port_peek(stream) == '\n' ||
	    port_peek(stream) == '#')
		return -1;

	goto st_tok; /* parse using a state machine */

st_tok:
	c = port_getc(stream);

	if (is_quote(c)) {
		quote = c;
		escape_char = 0;

		if (c == '\'')
			*out_should_expand = EXPAND_NONE;
		else if (c == '`')
			*out_should_expand = EXPAND_EVAL;

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
		} else
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
	if (port_eof(stream) || is_eot(port_peek(stream))) {
		if (len)
			goto st_accept;
		else
			return -1;
	} else {
		c = port_getc(stream);

		goto st_word;
	}

st_string:
	if (port_eof(stream))
		return -1;

	c = port_getc(stream);
	if (!escape_char && c == quote) {
		/* check that the very next char is not another string */
		if (is_quote(port_peek(stream)))
			reportret(-2, "strings too close together");

		goto st_accept;
	} else if (!escape_char && c == '\\') {
		escape_char = 1;
		goto st_string;
	} else if (escape_char && is_esc(c)) {
		escape_char = 0;
		for (i = 0; i < LEN(escs); i++)
			if (escs[i][0] == c) {
				TOK(escs[i][1]);
				goto st_string;
			}
		reportret(-2, "impossible escape");
	} else {
		if (escape_char)
			reportret(-2, "escaped a non-escapable char");
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
	char tok_buf[TOK_MAX], **tokens, *result;
	int i = 0, len, should_expand;
	string_port port;

	tokens = region_malloc(r, sizeof(char*)*MAX_TOKS_PER_LINE);

	skip_spaces_and_comments(stream);

	while ((len = read_token(tok_buf, stream, &should_expand)) > -1) {
		switch (should_expand) {
		case EXPAND_DEFAULT:
			if (!(tokens[i] = expand_variables(r, tok_buf, len)))
				return NULL;
			break;
		case EXPAND_NONE:
			tokens[i] = region_malloc(r, len + 1);
			strncpy(tokens[i], tok_buf, len + 1);
			break;
		case EXPAND_EVAL:
			port = (string_port){ .kind=STRPORT_CHAR, .text=tok_buf, .place=0 };
			if (!parse_and_execute(&port, &result)) /* TODO fix result memory leak */
				tokens[i] = result;
			else {
				efree(result);
				reportret(NULL, "eval failed");
			}
			break;
		}

		if (++i >= MAX_TOKS_PER_LINE)
			reportret(NULL, "line too long");
	}

	if (len == -2)
		return NULL;

	tokens[i] = NULL;

	return tokens;
}
