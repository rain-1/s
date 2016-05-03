#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "parser.h"
#include "interpreter.h"

void interpret_command(char **envp, struct AST* n)
{
	assert(n->type == NODE_COMMAND);

	execvpe(n->node.tokens[0], n->node.tokens, envp);
}

void interpret_pipe(char **envp, struct AST* n)
{
	if (n->type == NODE_COMMAND) {
		interpret_command(envp, n);
	}

	int fd[2];
	int f;

	pipe(&fd[0]);

	f = fork();
	if (f == -1) {
		fprintf(stderr, "fork() failed");
		exit(1);
	} else if (f == 0) { // child
		close(fd[1]);
		close(STDIN_FILENO);
		dup(fd[0]);
		close(fd[0]);
		interpret_pipe(envp, n->node.child.r);
	} else {	     // parent
		close(fd[0]);
		close(STDOUT_FILENO);
		dup(fd[1]);
		close(fd[1]);
		interpret_command(envp, n->node.child.l);
	}
}

void interpret(char **envp, struct AST* n)
{
	interpret_pipe(envp, n);
}
