#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "region.h"
#include "reporting.h"
#include "stringport.h"
#include "tokenizer.h"
#include "variables.h"

char varname[TOK_MAX];
char *varerr;

char *
expand_variables(region *r, char *tok, int t)
{
	char *stok = tok, *o, *val;
	int alloc_len = t+1;
	int i = 0, l;

	o = region_malloc(r, alloc_len);

	while (*tok) {
		if (*tok == '$') {
			if (!(tok = read_variable_prefix(tok)))
				reportret(NULL, "problem parsing variable inside token '%s' at character %d: %s", stok, i, varerr);

			if (!(val = getenv(varname)))
				reportret(NULL, "reference to an undefined variable inside token '%s' at character %d", stok, i);

			l = strlen(val);
			alloc_len += l;

			if (alloc_len > TOK_MAX)
				reportret(NULL, "variable expansion blew up token size too large");

			o = region_realloc(r, o, alloc_len);
			memcpy(o + i, val, l);
			i += l;
		} else {
			o[i++] = *tok++;
		}
	}
	o[i] = '\0';

	return o;
}

int
variable_character(char c)
{
	return c == '_'
		|| ('A' <= c && c <= 'Z')
		|| ('a' <= c && c <= 'z')
		|| ('0' <= c && c <= '9');
}

char *
read_variable_prefix(char *tok)
{
	int i = 0;
	int bracket = 0;

	assert(*tok == '$');
	tok++;

	/* NOTE: We don't bother to bounds check here */
	/* because tok is already <= the size of a token */
	/* ...lets see if this ever bites? */

	if (*tok == '{') {
		bracket = 1;
		tok++;
	}

	while (variable_character(*tok)) {
		if (i == 0 && ('0' <= *tok && *tok <= '9') && tok[1])
			reportvar(varerr, "variable can not start with a digit");
		varname[i++] = *tok++;
	}

	if (bracket && *tok++ != '}')
		reportvar(varerr, "missing '}'");

	varname[i] = '\0';

	if (!i)
		reportvar(varerr, "length 0 variable");

	return tok;
}
