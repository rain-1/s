#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#ifdef __linux__
#include <linux/limits.h>
#endif

#include "reporting.h"
#include "region.h"
#include "stringport.h"
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
		else if (!strcmp("source", n->node.tokens[0]))
			builtin_source(n->node.tokens);
		else if (!strcmp("exit", n->node.tokens[0]))
			builtin_exit(n->node.tokens);
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
			report("invalid $HOME\n");
			return;
		}
	}

	if (chdir(dir)) {
		report("could not change directory to [%s]\n", dir);
	}
	else {
		getcwd(cwd, PATH_MAX);
		setenv("PWD", cwd, 1);
	}
}

void builtin_set(char **argv) {
	if (argv[1] && argv[2])
		setenv(argv[1], argv[2], INT_MAX);
	else
		report("set requires two arguments\n");
}

void builtin_unset(char **argv) {
	if (argv[1])
		unsetenv(argv[1]);
	else
		report("unset requires an argument\n");
}

void builtin_source(char **argv) {
	FILE *f;
	int mode;

	if (!argv[1]) {
		report("source requires an argument\n");
	}
	else if (!(f = fopen(argv[1], "r"))) {
		report("source open() failed\n");
		return;
	}

	mode = interactive_mode;
	interactive_mode = 0;
	loop(f);
	interactive_mode = mode;
}

void builtin_exit(char **argv)
{
	if (!argv[1])
		exit(0);
	else
		exit(strtol(argv[1], NULL, 0));
}
