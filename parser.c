/* see LICENSE file for copyright and license details */
/* call tokenizer from stringport, generating an AST */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "region.h"
#include "stringport.h"
#include "tokenizer.h"
#include "parser.h"
#include "reporting.h"

char *operator_for[] = {
	[NODE_PIPE] = "|",
	[NODE_CONJ] = "&&",
	[NODE_DISJ] = "||",
};

static struct AST *
parse_binop(region *r, char **toks, NodeType ty)
{
	char **stoks = toks;
	struct AST* n;
	struct AST* m;

	if (ty == NODE_COMMAND) {
		if (!toks[0])
			reportret(NULL, "zero-length command");
		n = region_malloc(r, sizeof(struct AST));
		n->type = NODE_COMMAND;
		n->node.tokens = stoks;

		return n;
	}

	while (toks[0]) {
		if (!strcmp(operator_for[ty], toks[0])) {
			toks[0] = NULL;

			m = region_malloc(r, sizeof(struct AST));
			m->type = ty;
			if (!(m->node.child.l = parse_binop(r, stoks, ty-1)))
				return NULL;
			if (!(m->node.child.r = parse_binop(r, toks+1, ty)))
				return NULL;

			return m;
		} else {
			toks++;
		}
	}

	return parse_binop(r, stoks, ty-1);
}

static struct AST *
parse_tokens(region *r, char **toks, int *bg)
{
	int tokc = 0;

	while (toks[tokc])
		tokc++;

	if (tokc > 0 && !strcmp("&", toks[--tokc])) {
		*bg = 1;
		toks[tokc] = NULL;
	} else {
		*bg = 0;
	}

	return parse_binop(r, toks, NODE_DISJ);
}

struct AST *
parse(region *r, string_port *port, int *bg)
{
	char **toks = read_tokens(r, port);

	if (!toks)
		return NULL;
	if (toks[0])
		return parse_tokens(r, toks, bg);

	return NULL;
}
