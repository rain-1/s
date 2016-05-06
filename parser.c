#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "parser.h"

char *operator_for[] = {
	[NODE_PIPE] = "|",
	[NODE_CONJ] = "&&",
	[NODE_DISJ] = "||",
};

void free_ast_commands(char **r)
{
	while(*r) {
		free(*r);
		r++;
	}
}

char** free_ast_loop(struct AST *n)
{
	char **r;

	if (n->type == NODE_COMMAND) {
		r = n->node.tokens;
		free_ast_commands(r);
		free(n);
		return r;
	}
	else {
		free_ast_loop(n->node.child.r);
		r=free_ast_loop(n->node.child.l);
		free(n);
		return r;
	}
}

void free_ast(struct AST *n)
{
	free(free_ast_loop(n));
}

struct AST* parse_binop(char **tokens, NodeType ty)
{
	char **stokens;
	struct AST* n;
	struct AST* m;

	stokens = tokens;

	if (ty == NODE_COMMAND) {
		if (!tokens[0]) {
			fprintf(stderr, "Error: zero-length command\n");
			exit(-1);
		}
		n = malloc(sizeof(struct AST));
		n->type = NODE_COMMAND;
		n->node.tokens = stokens;

		return n;
	}

	while (tokens[0]) {
		if (!strcmp(operator_for[ty], tokens[0])) {
			free(tokens[0]);
			tokens[0] = NULL;

			m = malloc(sizeof(struct AST));
			m->type = ty;
			m->node.child.l = parse_binop(stokens, ty-1);
			m->node.child.r = parse_binop(tokens+1, ty);

			return m;
		}
		else {
			tokens++;
		}
	}

	return parse_binop(stokens, ty-1);
}

struct AST* parse_tokens(char **tokens, int *bg_flag)
{
	int i = 0;

	while (tokens[i])
		i++;
        if (i > 0 && !strcmp("&", tokens[i-1])) {
		*bg_flag=1;
		free(tokens[i-1]);
		tokens[i-1] = NULL;
	}
	else {
		*bg_flag=0;
	}

	return parse_binop(tokens, NODE_DISJ);
}

struct AST* parse(FILE *f, int *bg_flag)
{
	char **tokens = read_tokens(f);

	if (tokens[0]) {
		return parse_tokens(tokens, bg_flag);
	}
	else {
		return NULL;
	}
}
