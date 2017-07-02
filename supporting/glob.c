#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glob.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
	int i;
	glob_t g;

	if (argc < 2) {
		fprintf(stderr, "glob: invoked with no arguments\n");
		return EXIT_FAILURE;
	}

	for (i = 1; i < argc; i++)
		switch (glob(argv[i], (i-1 ? GLOB_APPEND : 0) |
		             (strchr(argv[i], '*') ? 0 : GLOB_NOCHECK),
		             NULL, &g)) {
		case GLOB_NOSPACE:
			fprintf(stderr, "glob: running out of memory\n");
		case GLOB_ABORTED:
			fprintf(stderr, "glob: read error\n");
		case GLOB_NOMATCH:
			fprintf(stderr, "glob: no matches\n");
			return EXIT_FAILURE;
		}

	if (!g.gl_pathv[0]) {
		fprintf(stderr, "glob: NULL\n");
		return EXIT_FAILURE;
	}

	execvp(g.gl_pathv[0], g.gl_pathv);

	fprintf(stderr, "glob: could not execute %s\n", g.gl_pathv[0]);
	return EXIT_FAILURE;
}
