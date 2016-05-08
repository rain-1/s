#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tokenizer.h"
#include "parser.h"

void display_ast(struct AST* n)
{
	char **t;

	switch(n->type) {
	case NODE_COMMAND:
		puts("NODE COMMAND");
		t = n->node.tokens;
		while (*t) {
			printf("%s ", *t);
			t++;
		}
		puts("");

		break;
	default:
		printf("NODE %s", operator_for[n->type]);
		puts("LEFT:");
		display_ast(n->node.child.l);
		puts("RIGHT:");
		display_ast(n->node.child.r);
		puts("");
		break;
	}

	puts("");
}

int main(int argc, char **argv) {
	struct AST* n;
	int i, b;
	FILE *f;
	
	char *x, *y;

	if (argc == 1)
		f = stdin;
	else {
		printf("opening %s\n", argv[1]);
		f = fopen(argv[1], "r");
	}

	do {
		n = parse(f, &b, &x, &y);
		display_ast(n);

		skip_newline(f);
	} while(!feof(f));

	return 0;
}
