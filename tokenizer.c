#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"

// from stackoverflow.com/questions/2082743/c-equivalent-to-fstreams-peek
int fpeek(FILE *stream)
{
	int c;

	c = fgetc(stream);
	ungetc(c, stream);

	return c;
}

void skip_spaces()
{
	while (fpeek(stdin) == ' ')
		fgetc(stdin);
}

void token()
{
	skip_spaces();
}
