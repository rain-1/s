#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "reporting.h"
#include "region.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
#include "builtins.h"

int interactive_mode = 0;

void interpret_command(struct AST* n)
{
	assert(n->type == NODE_COMMAND);

	execvp(n->node.tokens[0], n->node.tokens);

	_reporterr("Error: Could not execute the program named [%s]\n", n->node.tokens[0]);
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
		_reporterr("fork() failure");
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
			_exit(WEXITSTATUS(r));
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
		_reporterr("fork() failure");
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

void prompt()
{
	if (interactive_mode) {
		printf("%s", geteuid() == 0 ? "s# " : "s$ ");
		fflush(stdout);
	}
}

void loop(FILE *f) {
	pid_t p;
	int bg;
	region r;
	struct AST* n;
	int status;

	do {
		prompt();

		region_create(&r);

		n = parse(&r, f, &bg);

		if (n && !perform_builtin(n)) {
			if (!(p = fork())) {
				interpret(n);
				_reporterr("== SHOULD NEVER GET HERE ==");
			}

			if (!bg) {
				waitpid(p, &status, 0);
			}
		}

		region_free(&r);

		skip_newline(f);
	} while(!feof(f));
}
