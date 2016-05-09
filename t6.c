#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reporting.h"
#include "region.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
#include "builtins.h"

void prompt_()
{
	if (interactive_mode) {
		printf("%s", geteuid() == 0 ? "s# " : "s$ ");
		fflush(stdout);
	}
}

int main(int argc, char **argv)
{
	struct AST* n;
	int i, b;
	FILE *f;

	int status;
	pid_t p;

	region r;

	if (argc == 1) {
		f = stdin;

		interactive_mode = isatty(fileno(stdin));
	}
	else {
		f = fopen(argv[1], "r");
		if (!f) {
			fprintf(stderr, "Could not open file <%s>!", argv[1]);
			exit(-1);
		}

		interactive_mode = 0;
	}


	do {
		prompt_();

		region_create(&r);
		n = parse(&r, f, &b);
		region_free(&r);

		skip_newline(f);
	} while(!feof(f));

	return 0;
}
