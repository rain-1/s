#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "parser.h"
#include "interpreter.h"

char cwd[PATH_MAX];

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

	prompt();
	do {
		n = parse(f, envp);

		if (n->type == NODE_COMMAND && n->node.tokens[0] && !strncmp(n->node.tokens[0], "cd", 2)) {
			// TODO if [1] is NULL go to $HOME instead

			if (chdir(n->node.tokens[1])) {
				fprintf(stderr, "Error: Could not change directory to [%s]\n", n->node.tokens[1]);
			}
			else {
				getcwd(cwd, PATH_MAX);
				setenv("PWD", cwd, 1);
			}
		}
		else {
			if (!(p = fork())) {
				interpret(n, envp);
				puts("== SHOULD NEVER GET HERE ==");
				return -1;
			}
			waitpid(p, &status, 0);
		}

		free_ast(n);

		prompt(); // TODO: why does this have to be before skip newline?
		skip_newline(f);
	} while(!feof(f));

	return 0;
}
