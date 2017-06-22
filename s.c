#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "region.h"
#include "reporting.h"
#include "stringport.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
#include "builtins.h"

void
handler_sigint(int sig)
{
  /* signal(sig, SIG_IGN); */
}

int
main(int argc, char **argv)
{
	int i;
	FILE *f;

	signal(SIGINT, handler_sigint);

	setenv("SHELL", "/bin/s", 1);

	if (argc == 1) {
		f = stdin;

		interactive_mode = isatty(fileno(stdin));
	} else {
		if (!(f = fopen(argv[1], "r")))
			reporterr("Could not open file [%s]!", argv[1]);

		interactive_mode = 0;
	}

	interpreter_loop(f);

	return 0;
}
