#include <stdlib.h>
#include <string.h>

#include "variables.h"

char *expand_variables(char *tok, int t, char **envp)
{
	char *o;

	o = malloc(t+1);
	memcpy(o, tok, t+1);

	return o;
}
