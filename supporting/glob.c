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
	int res;

	argc--;
	argv++;

	g.gl_offs = 0;

	for (i = 0; i < argc; i++) {
		res = glob(argv[i], GLOB_DOOFFS | (i ? GLOB_APPEND : 0) | (strchr(argv[i], '*') ? 0 : GLOB_NOCHECK), NULL, &g);
		if (!res) {
			if (res == GLOB_NOSPACE) {
				fprintf(stderr, "glob: GLOB_NOSPACE - running out of memory\n");
				return EXIT_FAILURE;
			} else if (res == GLOB_ABORTED) {
				fprintf(stderr, "glob: GLOB_ABORTED - read error\n");
				return EXIT_FAILURE;
			} else if (res == GLOB_NOMATCH) {
				fprintf(stderr, "glob: GLOB_NOMATCH - no matches\n");
				return EXIT_FAILURE;
			}
		}
	}

	if (!g.gl_pathv[0]) {
		fprintf(stderr, "glob: NULL\n");
		return EXIT_FAILURE;
	}

	execvp(g.gl_pathv[0], g.gl_pathv);

	fprintf(stderr, "Error: could not execute %s\n", g.gl_pathv[0]);
	return EXIT_FAILURE;
}
