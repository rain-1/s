/* see LICENSE file for copyright and license details */
/* user commands built directly into s */
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
#include "util.h"

char cwd[PATH_MAX];
char owd[PATH_MAX];

Builtin builtins[] = {
	{ "source", &builtin_source },
	{ "cd",     &builtin_cd },
	{ "set",    &builtin_set },
	{ "unset",  &builtin_unset },
	{ "exit",   &builtin_exit },
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
builtin_source(char **argv)
{
	FILE *f;
	int mode;

	if (!argv[1])
		reportret(,"%s: argument required", argv[0]);
	if (!(f = fopen(argv[1], "r")))
		reportret(,"%s: %s: could not load file", argv[0], argv[1]);

	mode = interactive_mode;
	interactive_mode = 0;

	vars_set(argv);

	interpreter_loop(f);

	interactive_mode = mode;
}

void
builtin_cd(char **argv)
{
	char *dir;
	int isowd = 0;

	if (!(dir = argv[1])) {
		if (!(dir = getenv("HOME")))
			reportret(,"%s: invalid $HOME", argv[0]);
	} else if (strcmp(dir, "-") == 0) {
		if (!(dir = getenv("OWD")))
			reportret(,"%s: invalid $OWD", argv[0]);
		isowd = 1;
	}

	getcwd(owd, PATH_MAX);
	if (chdir(dir)) {
		report("%s: %s: could not change to directory", argv[0], dir);
	} else {
		getcwd(cwd, PATH_MAX);
		setenv("PWD", cwd, 1);
		setenv("OWD", owd, 1);
		if (isowd)
			printf("%s\n", dir);
	}
}

void
builtin_set(char **argv)
{
	if (argv[1] && argv[2])
		setenv(argv[1], argv[2], INT_MAX);
	else
		report("%s: two arguments required", argv[0]);
}

void
builtin_unset(char **argv)
{
	if (argv[1])
		unsetenv(argv[1]);
	else
		report("%s: argument required", argv[0]);
}

void
builtin_exit(char **argv)
{
	if (!argv[1])
		exit(0);
	else
		exit(strtol(argv[1], NULL, 0));
}
