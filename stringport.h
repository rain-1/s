/* see LICENSE file for copyright and license details */

typedef enum { STRPORT_CHAR, STRPORT_FILE } Strport;

typedef struct {
  Strport kind;

  /* kind=STRPORT_CHAR */
  char *text;
  int place;

  /* kind=STRPORT_FILE */
  FILE *fptr;
} string_port;

int port_peek(string_port *port);
int port_eof(string_port *port);
int port_getc(string_port *port);

