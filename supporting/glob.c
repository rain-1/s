#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glob.h>

int main(int argc, char **argv) {
	int i;
	glob_t g;

	argc--;
	argv++;

	g.gl_offs = 0;

	for (i = 0; i < argc; i++) {
		glob(argv[i], GLOB_DOOFFS | (i ? GLOB_APPEND : 0) | (strchr(argv[i], '*') ? 0 : GLOB_NOCHECK), NULL, &g);
	}

	execvp(g.gl_pathv[0], g.gl_pathv);

	fprintf(stderr, "Error: could not execute %s\n", g.gl_pathv[0]);
	return 1;
}
