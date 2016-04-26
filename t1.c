#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"

int main(void) {
	printf("%d\n", token(stdin));
	printf("%s\n", tok_buf);
	printf("%d\n", token(stdin));
	printf("%s\n", tok_buf);

	return 0;
}
