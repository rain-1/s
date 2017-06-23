#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "reporting.h"
#include "region.h"
#include "stringport.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
#include "builtins.h"

#include "linenoise.h"

int interactive_mode = 0;

void
interpret_command(struct AST* n)
{
	assert(n->type == NODE_COMMAND);

	execvp(n->node.tokens[0], n->node.tokens);

	_reporterr("Error: Could not execute the program named [%s]", n->node.tokens[0]);
}

void
interpret_junction(struct AST* n)
{
	pid_t p;
	int r;

	if (n->type == NODE_COMMAND)
		interpret_command(n);

	switch (p = fork()) {
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

void
interpret_pipe(struct AST* n)
{
	if (n->type == NODE_COMMAND)
		interpret_command(n);

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

void
interpret(struct AST* n)
{
	switch (n->type) {
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

int
prompt(string_port *port)
{
	char *line;

	if (interactive_mode) {
		if ((line = linenoise(geteuid() == 0 ? "s# " : "s$ "))) {
			*port = (string_port){ .kind=STRPORT_CHAR, .text=line, .place=0 };
			return 0;
		}

		return 1;
	}

	return 0;
}

void
drain_pipe(int fd, char **out) {
	int len = 0;
	int size = 1000;
	char *str = malloc(size);
	int n;
	
	while ((n = read(fd, str + len, size - 1 - len)) > 0) {
		len += n;
		
		if (len > size/2) {
			size *= 2;
			str = realloc(str, size);
		}
	}
	
	str[len] = '\0';
	
	*out = str;
}

void
parse_and_execute(string_port *port, char **string_capture) {
	pid_t p;
	region r;
	struct AST *n;
	int bg;
	int status;

	int fd[2];
	
	region_create(&r);
	
	n = parse(&r, port, &bg);
	
	if (n && !perform_builtin(n)) {
		if (string_capture) {
			pipe(&fd[0]);
		}
		
		if (!(p = fork())) {
			if (string_capture) {
				close(fd[0]);
				close(STDOUT_FILENO);
				dup(fd[1]);
			}
			
			interpret(n);
			_reporterr("== SHOULD NEVER GET HERE ==");
		}
		
		if (string_capture) {
			close(fd[1]);
			drain_pipe(fd[0], string_capture);
			close(fd[0]);
		}
		
		if (!bg)
			waitpid(p, &status, 0);
	}
	
	region_free(&r);
}

void
interpreter_loop(FILE *f)
{
	string_port port;

	if (!interactive_mode)
		port = (string_port){ .kind=STRPORT_FILE, .fptr=f };

	do {
		if (prompt(&port)) {
			if (errno == EAGAIN) {
				errno = 0;
				continue;
			}
			else
				break;
		}
		
		parse_and_execute(&port, NULL);
		
		if (interactive_mode) {
			// TODO: Only add if command was sucessful?
			linenoiseHistoryAdd(port.text);
			free(port.text);
		} else {
			skip_newline(&port);
		}
	} while (!feof(f));
}
