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
	int i;
	FILE *f;

	int status;
	pid_t p;

	int bg;
	char *redir_in, *redir_out;

	setenv("SHELL", "/bin/s", 1);

	if (argc == 1) {
		f = stdin;

		interactive_mode = isatty(fileno(stdin));
	}
	else {
		f = fopen(argv[1], "r");
		if (!f) {
			reporterr("Could not open file <%s>!", argv[1]);
			exit(-1);
		}

		interactive_mode = 0;
	}


	do {
		prompt();

		n = parse(f, &bg, &redir_in, &redir_out);

		if (n && !perform_builtin(n)) {
			if (!(p = fork())) {
				interpret(n);
				puts("== SHOULD NEVER GET HERE ==");
				return -1;
			}

			if (!bg) {
				waitpid(p, &status, 0);
			}
		}

		if (n)
			free_ast(n);

		skip_newline(f);
	} while(!feof(f));

	return 0;
}
