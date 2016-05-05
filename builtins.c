#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "parser.h"
#include "builtins.h"

char cwd[PATH_MAX];

int perform_builtin(struct AST *n)
{
	if (n->type == NODE_COMMAND && n->node.tokens[0]) {
		if (!strncmp(n->node.tokens[0], "cd", strlen("cd"))) builtin_cd(n->node.tokens);
		else return 0;

		return 1;
	}

	return 0;
}

void builtin_cd(char **args)
{
	char *dir;

	if (!(dir = args[1])) {
		// TODO getpwnam to cd ~
	}

	if (chdir(dir)) {
		fprintf(stderr, "Error: Could not change directory to [%s]\n", dir);
	}
	else {
		getcwd(cwd, PATH_MAX);
		setenv("PWD", cwd, 1);
	}
}
