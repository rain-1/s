/* see LICENSE file for copyright and license details */
/* expand variables into their stored value */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "region.h"
#include "reporting.h"
#include "stringport.h"
#include "tokenizer.h"
#include "variables.h"
#include "util.h"

char varname[TOK_MAX];
char *varerr;

/* set positional variables before loading file */
void
vars_set(char **argv)
{
	int i = 0;
	char var[8];
	/* char varcat[ARG_MAX_STRLEN]; */
	long max = sysconf(_SC_ARG_MAX);

	for (argv++; *argv && i < max; argv++, i++) {
		sprintf(var, "%d", i);
		setenv(var, *argv, 1);
	}
	setenv("#", var, 1);
}

/* remove positional variables to prevent leakage */
void
vars_unset(void)
{
	int i = 0;
	char var[8];
	long max = sysconf(_SC_ARG_MAX);

	for (; i < max; i++) {
		sprintf(var, "%d", i);
		if (!getenv(var)) break;
		unsetenv(var);
	}
	unsetenv("#");
}

static int
variable_character(char c)
{
	return c == '_' ||
	       BETWEEN(c, 'A', 'Z') ||
	       BETWEEN(c, 'a', 'z') ||
	       BETWEEN(c, '0', '9');
}

static char *
read_variable_prefix(char *tok)
{
	int pos = 0;
	int brc = 0;

	assert(*tok == '$');
	tok++;

	/* NOTE: We don't bother to bounds check here */
	/* because tok is already <= the size of a token */
	/* ...lets see if this ever bites? */

	if (*tok == '{') {
		brc = 1;
		tok++;
	}

	while (variable_character(*tok) || (!pos && *tok == '#'))
		varname[pos++] = *tok++;

	if (brc && *tok++ != '}')
		reportvar(varerr, "missing '}'");

	varname[pos] = '\0';

	if (!pos)
		reportvar(varerr, "length 0 variable");

	return tok;
}

char *
expand_variables(region *r, char *tok, int t)
{
	char *stok = tok, *o, *val;
	int alloc_len = t+1;
	int pos = 0, l;

	o = region_malloc(r, alloc_len);

	while (*tok)
		if (*tok == '$') {
			if (!(tok = read_variable_prefix(tok)))
				reportret(NULL, "problem parsing variable '%s' at character %d: %s",
				          stok, pos, varerr);

			if (!(val = getenv(varname)))
				reportret(NULL, "reference to undefined variable '%s'", stok);

			l = strlen(val);
			alloc_len += l;

			if (alloc_len > TOK_MAX)
				reportret(NULL, "variable expansion blew up token size too large");

			o = region_realloc(r, o, alloc_len);
			memcpy(o + pos, val, l);
			pos += l;
		} else {
			o[pos++] = *tok++;
		}
	o[pos] = '\0';

	return o;
}
