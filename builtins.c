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
char owd[PATH_MAX];

Builtin builtins[] = {
	{ "cd",     &builtin_cd },
	{ "set",    &builtin_set },
	{ "unset",  &builtin_unset },
	{ "source", &builtin_source },
	{ "exit",   &builtin_exit },
	{ "eval",   &builtin_eval },
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
	int isowd = 0;

	if (!(dir = args[1])) {
		if (!(dir = getenv("HOME")))
			reportret(,"invalid $HOME");
	} else if (strcmp(dir, "-") == 0) {
		if (!(dir = getenv("OLDPWD")))
			reportret(,"invalid $OLDPWD");
		isowd = 1;
	}

	getcwd(owd, PATH_MAX);
	if (chdir(dir)) {
		report("could not change directory to [%s]", dir);
	} else {
		getcwd(cwd, PATH_MAX);
		setenv("PWD", cwd, 1);
		setenv("OLDPWD", owd, 1);
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

	if (!argv[1])
		reportret(,"source requires an argument");
	if (!(f = fopen(argv[1], "r")))
		reportret(,"source open() failed");

	mode = interactive_mode;
	interactive_mode = 0;
	interpreter_loop(f);
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

void
builtin_eval(char **argv) {
	string_port port;
	
	if (argv[1] && argv[2]) {
		port = (string_port){ .kind=STRPORT_CHAR, .text=argv[2], .place=0 };
		parse_and_execute(&port);

		// TODO: get the output of execute as a string and put it into the variable
	}
	else {
		report("eval requires two+ arguments: a var and a command");
	}
}
