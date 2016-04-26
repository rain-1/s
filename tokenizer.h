#define TOK_MAX 4096

extern char tok_buf[TOK_MAX];

void skip_spaces(FILE *stream);
void skip_newline(FILE *stream);
int token(FILE *stream);

