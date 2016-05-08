#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "reporting.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
#include "builtins.h"

void prompt()
{
	if (interactive_mode) {
		printf("%s", geteuid() == 0 ? "s# " : "s$ ");
		fflush(stdout);
	}
}

int main(int argc, char **argv)
{
	struct AST* n;
	int i, b;
	FILE *f;

	int status;
	pid_t p;

	setenv("SHELL", "/bin/s", 1);

	if (argc == 1) {
		f = stdin;

		interactive_mode = isatty(fileno(stdin));
	}
	else {
		f = fopen(argv[1], "r");
		if (!f) {
			fprintf(stderr, "Could not open file <%s>!", argv[1]);
			exit(-1);
		}

		interactive_mode = 0;
	}


	do {
		prompt();

		n = parse(f, &b);

		if (n && !perform_builtin(n)) {
			if (!(p = fork())) {
				interpret(n);
				puts("== SHOULD NEVER GET HERE ==");
				return -1;
			}

			if (!b) {
				waitpid(p, &status, 0);
			}
		}

		if (n)
			free_ast(n);

		skip_newline(f);
	} while(!feof(f));

	return 0;
}
