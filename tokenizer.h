#define TOK_MAX 4096

extern char tok_buf[TOK_MAX];

void skip_spaces(FILE *stream);
int token(FILE *stream);

