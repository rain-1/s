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

#define LEN(X) (sizeof(X) / sizeof((X)[0]))

char cwd[PATH_MAX];

Builtin builtins[] = {
	{ "cd",     &builtin_cd },
	{ "set",    &builtin_set },
	{ "unset",  &builtin_unset },
	{ "source", &builtin_source },
	{ "exit",   &builtin_exit }
};

int
perform_builtin(struct AST *n)
{
	int i;

	if (n->type != NODE_COMMAND || !n->node.tokens[0])
		return 0;

	for (i = 0; i < LEN(builtins); i++)
		if (!strcmp(builtins[i].name, n->node.tokens[0])) {
			(*builtins[i].func)(n->node.tokens);
			return 1;
		}

	return 0;
}

void
builtin_cd(char **args)
{
	char *dir;

	if (!(dir = args[1]))
		if (!(dir = getenv("HOME"))) {
			report("invalid $HOME");
			return;
		}

	if (chdir(dir)) {
		report("could not change directory to [%s]", dir);
	} else {
		getcwd(cwd, PATH_MAX);
		setenv("PWD", cwd, 1);
	}
}

void
builtin_set(char **argv)
{
	if (argv[1] && argv[2])
		setenv(argv[1], argv[2], INT_MAX);
	else
		report("set requires two arguments");
}

void
builtin_unset(char **argv)
{
	if (argv[1])
		unsetenv(argv[1]);
	else
		report("unset requires an argument");
}

void
builtin_source(char **argv)
{
	FILE *f;
	int mode;

	if (!argv[1]) {
		report("source requires an argument");
		return;
	} else if (!(f = fopen(argv[1], "r"))) {
		report("source open() failed");
		return;
	}

	mode = interactive_mode;
	interactive_mode = 0;
	loop(f);
	interactive_mode = mode;
}

void
builtin_exit(char **argv)
{
	if (!argv[1])
		exit(0);
	else
		exit(strtol(argv[1], NULL, 0));
}
