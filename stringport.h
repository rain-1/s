/* see LICENSE file for copyright and license details */

#define STRPORT_CHAR 0
#define STRPORT_FILE 1

typedef struct {
  int kind;

  /* kind=STRPORT_CHAR */
  char *text;
  int place;

  /* kind=STRPORT_FILE */
  FILE *fptr;
} string_port;

int port_peek(string_port *port);
int port_eof(string_port *port);
int port_getc(string_port *port);

