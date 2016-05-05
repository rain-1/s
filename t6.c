#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "interpreter.h"
#include "builtins.h"

int display_prompt;

void prompt()
{
	if (display_prompt) {
		printf("%s", geteuid() == 0 ? "s# " : "s$ ");
		fflush(stdout);
	}
}

int main(int argc, char **argv, char **envp)
{
	struct AST* n;
	int i;
	FILE *f;

	int status;
	pid_t p;

	if (argc == 1) {
		f = stdin;

		display_prompt = isatty(fileno(stdin));
	}
	else {
		f = fopen(argv[1], "r");
		if (!f) {
			fprintf(stderr, "Could not open file <%s>!", argv[1]);
			exit(-1);
		}

		display_prompt = 0;
	}


	do {
		prompt();
		n = parse(f, envp);

		if (!perform_builtin(n)) {
			if (!(p = fork())) {
				interpret(n, envp);
				puts("== SHOULD NEVER GET HERE ==");
				return -1;
			}
			waitpid(p, &status, 0);
		}

		free_ast(n);

		skip_newline(f);
	} while(!feof(f));

	return 0;
}
