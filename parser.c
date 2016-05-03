#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "parser.h"

void free_ast(struct AST *n)
{
	// TODO
}

char** read_tokens(FILE *f)
{
	char **tokens;
	int i, t;

	i = 0;

	tokens = malloc(sizeof(char*)*MAX_TOKS_PER_LINE);

	while ((t = token(f)) != -1) {
		tokens[i] = malloc(t+1);
		memcpy(tokens[i], tok_buf, t+1);
		i++;
	}

	tokens[i] = NULL;

	return tokens;
}

struct AST* parse_pipe(char **tokens)
{
	struct AST* n;
	struct AST* m;

	n = malloc(sizeof(struct AST));
	n->type = NODE_COMMAND;
	n->node.tokens = tokens;

	while (tokens[0]) {
		if (!strcmp("|", tokens[0])) {
			free(tokens[0]);
			tokens[0] = NULL;

			m = malloc(sizeof(struct AST));
			m->type = NODE_PIPE;
			m->node.child.l = n;
			m->node.child.r = parse_pipe(tokens+1);

			return m;
		}
		else {
			tokens++;
		}
	}

	return n;
}

struct AST* parse_tokens(char **tokens)
{
	return parse_pipe(tokens);
}

struct AST* parse(FILE *f)
{
	return parse_tokens(read_tokens(f));
}
