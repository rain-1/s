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

	int status;
	pid_t p;

	if (argc == 1)
		f = stdin;
	else {
		printf("opening %s\n", argv[1]);
		f = fopen(argv[1], "r");
		if (!f) {
			fprintf(stderr, "Could not open file <%s>!", argv[1]);
			exit(-1);
		}
	}

	do {
		n = parse(f);
		if (!(p = fork())) {
			interpret(envp, n);
			puts("== SHOULD NEVER GET HERE ==");
			return -1;
		}
		free_ast(n);
		waitpid(p, &status, 0);

		skip_newline(f);
	} while(!feof(f));

	return 0;
}
