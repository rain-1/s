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

struct AST *
parse_binop(region *r, char **tokens, NodeType ty)
{
	char **stokens;
	struct AST* n;
	struct AST* m;

	stokens = tokens;

	if (ty == NODE_COMMAND) {
		if (!tokens[0])
			reportret(NULL, "bad syntax, zero-length command");
		n = region_malloc(r, sizeof(struct AST));
		n->type = NODE_COMMAND;
		n->node.tokens = stokens;

		return n;
	}

	while (tokens[0]) {
		if (!strcmp(operator_for[ty], tokens[0])) {
			tokens[0] = NULL;

			m = region_malloc(r, sizeof(struct AST));
			m->type = ty;
			if (!(m->node.child.l = parse_binop(r, stokens, ty-1)))
				return NULL;
			if (!(m->node.child.r = parse_binop(r, tokens+1, ty)))
				return NULL;

			return m;
		} else {
			tokens++;
		}
	}

	return parse_binop(r, stokens, ty-1);
}

struct AST *
parse_tokens(region *r, char **tokens, int *bg_flag)
{
	int i = 0;

	while (tokens[i])
		i++;

	if (i > 0 && !strcmp("&", tokens[i-1])) {
		*bg_flag = 1;
		tokens[--i] = NULL;
	} else {
		*bg_flag = 0;
	}

	return parse_binop(r, tokens, NODE_DISJ);
}

struct AST *
parse(region *r, string_port *port, int *bg_flag)
{
	char **tokens = read_tokens(r, port);

	if (!tokens)
		return NULL;
	if (tokens[0])
		return parse_tokens(r, tokens, bg_flag);

	return NULL;
}
