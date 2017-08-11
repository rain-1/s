/* see LICENSE file for copyright and license details */
/* command line interpreter */
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "arg.h"
#include "region.h"
#include "reporting.h"
#include "stringport.h"
#include "tokenizer.h"
#include "variables.h"
#include "parser.h"
#include "interpreter.h"
#include "builtins.h"

char *argv0;
int debug = 0;

void
handler_sigint(int sig)
{
	sig = sig; /* get rid of compiler warnings from -Wextra */
	/* signal(sig, SIG_IGN); */
}

static void
usage(int eval)
{
	fprintf(stderr, "usage: %s [-dvh] [SCRIPT ...]\n", argv0);
	exit(eval);
}

int
main(int argc, char **argv)
{
	FILE *f;

	ARGBEGIN {
	case 'd':
		debug = 1;
		break;
	case 'v':
		printf("%s v%s (c) 2014, 2016, 2017 s team\n", argv0, VERSION);
		return 0;
	case 'h':
		usage(0);
	default:
		usage(1);
	} ARGEND;

	signal(SIGINT, handler_sigint);

	setenv("SHELL", "/bin/s", 1);

	if (argc == 1) {
		f = stdin;

		interactive_mode = isatty(fileno(stdin));
	} else {
		if (!(f = fopen(argv[1], "r")))
			reporterr("source: %s: could not load file", argv[1]);

		vars_set(argv);

		interactive_mode = 0;
	}

	interpreter_loop(f);

	return 0;
}
