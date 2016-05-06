#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#ifdef __linux__
#include <linux/limits.h>
#endif

#include "parser.h"
#include "interpreter.h"
#include "builtins.h"

char cwd[PATH_MAX];

int perform_builtin(struct AST *n)
{
	if (n->type == NODE_COMMAND && n->node.tokens[0]) {
		if (!strcmp("cd", n->node.tokens[0]))
			builtin_cd(n->node.tokens);
		else if (!strcmp("set", n->node.tokens[0]))
			builtin_set(n->node.tokens);
		else if (!strcmp("unset", n->node.tokens[0]))
			builtin_unset(n->node.tokens);
		else return 0;

		return 1;
	}

	return 0;
}

void builtin_cd(char **args)
{
	char *dir;

	if (!(dir = args[1])) {
		if (!(dir = getenv("HOME"))) {
			fprintf(stderr, "%s: invalid $HOME\n", interactive_mode ? "Warning" : "Error");
			if (!interactive_mode)
				exit(-1);
			else
				return;
		}
	}

	if (chdir(dir)) {
		fprintf(stderr, "%s: could not change directory to [%s]\n", interactive_mode ? "Warning" : "Error", dir);
		if (!interactive_mode)
			exit(-1);
	}
	else {
		getcwd(cwd, PATH_MAX);
		setenv("PWD", cwd, 1);
	}
}

void builtin_set(char **argv) {
	if (argv[1] && argv[2]) {
		setenv(argv[1], argv[2], INT_MAX);
	}
	else {
		fprintf(stderr, "%s: set requires two arguments\n", interactive_mode ? "Warning" : "Error");
		if (!interactive_mode)
			exit(-1);
	}
}

void builtin_unset(char **argv) {
	if (argv[1]) {
		unsetenv(argv[1]);
	}
	else {
		fprintf(stderr, "%s: unset requires an argument\n", interactive_mode ? "Warning" : "Error");
		if (!interactive_mode)
			exit(-1);
	}
}
