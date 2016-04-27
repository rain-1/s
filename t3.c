#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tokenizer.h"

char **tokens;

int main(int argc, char **argv, char **envp) {
        int t = 0;
        int i = 0;
	tokens = malloc(sizeof(char*)*420);
	
	FILE *f;
	
	if (argc == 1)
		f = stdin;
	else {
		printf("opening %s\n", argv[1]);
		f = fopen(argv[1], "r");
	}

	do {
		i = 0;
		
		// read tokens
		while ((t = token(f)) != -1) {
			// allocate space and copy them into the array
			tokens[i] = malloc(t+1);
			memcpy(tokens[i], tok_buf, t+1);
			//printf("[%s:%d]\n", tokens[i], t);
			i++;
		}
		tokens[i] = NULL;

		if (i == 0)
			continue;

		// fork off a child process
		if (!fork()) {
			execvpe(tokens[0], tokens, envp);
			puts("!!!!!!! GOT PAST EXECVE !!!!!!");
		}
		// wait for it to finish
		int status;
		while (wait(&status) > -1);

        	int n;
	        for (n = 0; n < i; n++) {
			free(tokens[n]);
			//printf("[%s]\n", tokens[n]);
		}
		//puts("");

		skip_newline(f);
	} while(!feof(f));

	return 0;
}
