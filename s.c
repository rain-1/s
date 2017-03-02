#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "region.h"
#include "reporting.h"
#include "stringport.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
#include "builtins.h"

int main(int argc, char **argv)
{
	int i;
	FILE *f;

	setenv("SHELL", "/bin/s", 1);

	if (argc == 1) {
		f = stdin;

		interactive_mode = isatty(fileno(stdin));
	}
	else {
		f = fopen(argv[1], "r");
		if (!f) {
			reporterr("Could not open file <%s>!", argv[1]);
			exit(-1);
		}

		interactive_mode = 0;
	}

	loop(f);

	return 0;
}
