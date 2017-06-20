#define TOK_MAX 4096
#define MAX_TOKS_PER_LINE 4096

extern char tok_buf[TOK_MAX];

void skip_spaces(string_port *stream);
void skip_newline(string_port *stream);
char** read_tokens(region *r, string_port *stream);
