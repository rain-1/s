#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"

void display_ast(struct AST* n)
{
	char **t;

	switch(n->type) {
	case NODE_TOKENS:
		puts("NODE TOKENS");
		t = n->node.tokens;
		while (*t) {
			printf("%s ", *t);
			t++;
		}
		puts("");

		break;
	case NODE_PIPE:
		puts("NODE PIPE");
		puts("LEFT:");
		display_ast(n->node.child.l);
		puts("RIGHT:");
		display_ast(n->node.child.r);
		puts("");
		break;
	}

	puts("");
}

int main(int argc, char **argv, char **envp) {
	struct AST* n;
	int i;
	FILE *f;

	if (argc == 1)
		f = stdin;
	else {
		printf("opening %s\n", argv[1]);
		f = fopen(argv[1], "r");
	}

	do {
		n = parse(f);
		display_ast(n);

		skip_newline(f);
	} while(!feof(f));

	return 0;
}
