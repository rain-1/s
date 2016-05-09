#define TOK_MAX 4096
#define MAX_TOKS_PER_LINE 4096

extern char tok_buf[TOK_MAX];

void skip_spaces(FILE *stream);
void skip_newline(FILE *stream);
void skip_comment(FILE *stream);
int token(FILE *stream);
char** read_tokens(region *r, FILE *stream);
