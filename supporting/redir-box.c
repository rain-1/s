#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

#define BUF_SIZE 2096

char buf[BUF_SIZE];

int
main(int argc, char **argv)
{
	int s;
	char *flags, *name;
	FILE *fin = stdin, *fout = stdout, *ftmp;

	if (argc != 2) {
		fprintf(stderr, "Error: Invoked with no arguments\n");
		return EXIT_FAILURE;
	}

	name = basename(argv[0]);
	if (!strcmp("<", name))       flags = "r";
	else if (!strcmp(">", name))  flags = "w+";
	else if (!strcmp(">>", name)) flags = "a+";
	else {
		fprintf(stderr, "Error: Invoked as unknown command [%s]\n", name);
		return EXIT_FAILURE;
	}

	if (!(ftmp = fopen(argv[1], flags))) {
		fprintf(stderr, "Could not open file\n");
		return EXIT_FAILURE;
	}

	if (flags[0] == 'r')
		fin = ftmp;
	else
		fout = ftmp;

	do {
		s = fread(buf, 1, BUF_SIZE, fin);

		if (fwrite(buf, 1, s, fout) != s)
			fprintf(stderr, "Writing error\n");
	} while(s != -1 && s);

	fclose(fout);

	return EXIT_SUCCESS;
}
