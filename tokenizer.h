/* see LICENSE file for copyright and license details */

#define TOK_MAX 4096
#define MAX_TOKS_PER_LINE 4096

#define is_quote(C) (C && strchr("\"'`", C))
#define is_eot(C) (C && strchr(" \t\n\r#", C))

extern char tok_buf[TOK_MAX];

void skip_newline(string_port *stream);
char** read_tokens(region *r, string_port *stream);
