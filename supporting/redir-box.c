#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>

#define BUF_SIZE 2096

char buf[BUF_SIZE];

enum { lt, gt, gtgt };

int main(int argc, char **argv) {
	int mode, s;
	char *flags;
	FILE *fin, *fout, *ftmp;

	char *name;

	if (argc != 2) {
		fprintf(stderr, "Error: Invoked with no arguments\n");
		return EXIT_FAILURE;
	}

	name = basename(argv[0]);
	if (!strcmp("<", name)) mode = lt;
	else if (!strcmp(">", name)) mode = gt;
	else if (!strcmp(">>", name)) mode = gtgt;
	else {
		fprintf(stderr, "Error: Invoked as unknown command [%s]\n", name);
		return EXIT_FAILURE;
	}

	fin = stdin;
	fout = stdout;

	switch (mode) {
	case lt:
		flags = "r";
		break;
	case gt:
		flags = "w";
		break;
	case gtgt:
		flags = "a";
		break;
	}

	if (!(ftmp = fopen(argv[1], flags))) {
		fprintf(stderr, "Could not open file\n");
		return EXIT_FAILURE;
	}

	if (mode == lt)
		fin = ftmp;
	else
		fout = ftmp;

	do {
		s = fread(buf, BUF_SIZE, 1, fin);

		if (s == -1 || s == 0)
			break;

		if (fwrite(buf, s, 1, fout) != s) {
			fprintf(stderr, "Writing error\n");
		}
	} while(1);

	fclose(fout);

	return EXIT_SUCCESS;
}
