#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"
#include "interpreter.h"

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
		if (!fork()) {
			interpret(envp, n);
			puts("== SHOULD NEVER GET HERE ==");
		}
		free_ast(n);

		skip_newline(f);
	} while(!feof(f));

	return 0;
}
