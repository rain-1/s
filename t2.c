#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"

char **tokens;

int main(void) {
        int t = 0;
        int i = 0;
	tokens = malloc(sizeof(char*)*420);
	
	// read tokens
	while ((t = token(stdin)) != -1) {
		// allocate space and copy them into the array
		tokens[i] = malloc(t+1);
		memcpy(tokens[i], tok_buf, t);
		i++;
	}
	
	// print them all out
        int n;
        for (n = 0; n < i; n++) {
		printf("%s\n", tokens[n]);
	}
	
	system(tokens[i]);

	return 0;
}
