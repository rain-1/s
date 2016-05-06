#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

#include "parser.h"
#include "interpreter.h"

int interactive_mode = 0;

void interpret_command(struct AST* n)
{
	assert(n->type == NODE_COMMAND);

	execvp(n->node.tokens[0], n->node.tokens);

	fprintf(stderr, "Error: Could not execute the program named [%s]\n", n->node.tokens[0]);
	exit(-1);
}

void interpret_junction(struct AST* n)
{
	pid_t p;
	int r;

	if (n->type == NODE_COMMAND) {
		interpret_command(n);
	}

	switch(p = fork()) {
	case -1: 
		fprintf(stderr, "fork() failure");
		break;
	case 0:
		interpret(n->node.child.l);
		break;
	default:
		waitpid(p, &r, 0);

		// DISJ and CONJ are dual
		// xor flips the boolean for us depending on the case we're in
		//
		// so: in a disj (conj) node we exit right away
		//     if the exit status is (isn't) true
		//     otherwise continue executing the disj (conj)
		//     chain.

		if ((!WEXITSTATUS(r)) ^ (n->type == NODE_CONJ)) {
			exit(WEXITSTATUS(r));
		} else {
			interpret(n->node.child.r);
		}
		break;
	}
}

void interpret_pipe(struct AST* n)
{
	if (n->type == NODE_COMMAND) {
		interpret_command(n);
	}

	int fd[2];
	int f;

	pipe(&fd[0]);

	f = fork();
	if (f == -1) {
		fprintf(stderr, "fork() failed");
		exit(1);
	} else if (f == 0) { // child
		close(fd[0]);
		close(STDOUT_FILENO);
		dup(fd[1]);
		close(fd[1]);
		interpret_command(n->node.child.l);
	} else {	     // parent
		close(fd[1]);
		close(STDIN_FILENO);
		dup(fd[0]);
		close(fd[0]);
		interpret_pipe(n->node.child.r);
	}
}

void interpret(struct AST* n)
{
	switch(n->type) {
	case NODE_COMMAND:
		interpret_command(n);
		break;
	case NODE_CONJ:
	case NODE_DISJ:
		interpret_junction(n);
		break;
	case NODE_PIPE:
		interpret_pipe(n);
		break;
	}
}
