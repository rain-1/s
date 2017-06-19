#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
token(string_port *stream, int *out_should_expand)
{
	int l;
	char c;

	*out_should_expand = 1; // everything has variables expanded except for single ' strings

	skip_spaces(stream);
	if (port_eof(stream) || port_peek(stream) == '\n')
		return -1;

	l = 0;
	// WARNING: fpeek must be done before feof
	//          otherwise it may not succeed
	while (!token_end(port_peek(stream)) && !port_eof(stream)) {
		c = port_getc(stream);
		
		if (c == '\\') {
			tok_buf[l] = port_getc(stream);
			l++;
			
			port_peek(stream);
			if (port_eof(stream))
				reporterr("end of file when interpreting \\");
		}
		else if (c == '"' || c == '\'') {
			if (c == '\'') {
				*out_should_expand = 0;
			}

			l = token_rest_of_string(stream, c);
			
			if(l == -1)
				return -1;
			
			break;
		}
		else {
			tok_buf[l] = c;
			l++;
		}
		
		if (l >= TOK_MAX-1)
			reporterr("Token too long!");
	}
	tok_buf[l] = '\0';
	
	if (port_peek(stream) == '#')
		skip_comment(stream);

	if (!l)
		return -1;

	return l;
}

int
token_rest_of_string(string_port *stream, char quote_type) {
	// This is an auxilliary helper function for "token" which parses a single token
	// It it called when token has seen an open quote (either " or ')
	// It's purpose is to read the rest of the string into buf
	// until it sees a close quote.

	int l = 0;
	int escape = 0;
	char c;
	
	while (!port_eof(stream)) {
		c = port_getc(stream);
		
		if (c == quote_type && !escape) {
		        return l;
		}
		else if (c == '\\') {
			escape = 1;
			continue;
		}
		else {
			if (escape) {
				switch (c) {
				case '\\':
					tok_buf[l] = '\\';
					break;
					
				case 'a':
					tok_buf[l] = '\a';
					break;
					
				case 'n':
					tok_buf[l] = '\n';
					break;
					
				case 'r':
					tok_buf[l] = '\r';
					break;
					
				case 't':
					tok_buf[l] = '\t';
					break;
					
				default:
					// TODO: how about an error for escaping something that
					// doesn't need to be escaped
					tok_buf[l] = c;
				}
			}
			else {
				tok_buf[l] = c;
			}
			
			escape = 0;
			l++;
		}
	}

	// the input ended before a close quote
	return -1;
}

char **
read_tokens(region *r, string_port *f)
{
	char **tokens;
	int i, t;
	int should_expand;

	i = 0;

	tokens = region_malloc(r, sizeof(char*)*MAX_TOKS_PER_LINE);

	while ((t = token(f, &should_expand)) != -1) {
		if (should_expand) {
			if (!(tokens[i] = expand_variables(r, tok_buf, t)))
				return NULL;
		}
		else {
			tokens[i] = region_malloc(r, t+1);
			strncpy(tokens[i], tok_buf, t);
		}

		i++;
		if (i >= MAX_TOKS_PER_LINE)
			reporterr("Line too long!");
	}

	tokens[i] = NULL;

	return tokens;
}
