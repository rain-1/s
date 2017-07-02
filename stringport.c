/* see LICENSE file for copyright and license details */
#include <stdio.h>
#include <stdlib.h>

#include "stringport.h"

/* from stackoverflow.com/questions/2082743/c-equivalent-to-fstreams-peek */
int
fpeek(FILE *stream)
{
	int c;

	c = fgetc(stream);
	ungetc(c, stream);

	return c;
}

int
port_peek(string_port *port)
{
	switch (port->kind) {
	case STRPORT_CHAR:
		return port->text[port->place];
	case STRPORT_FILE:
		return fpeek(port->fptr);
	default:
		exit(1);
	}
}

int
port_eof(string_port *port)
{
	switch (port->kind) {
	case STRPORT_CHAR:
		return port->text[port->place] == '\0';
	case STRPORT_FILE:
		return feof(port->fptr);
	default:
		exit(1);
	}
}

int
port_getc(string_port *port)
{
	int c;

	switch (port->kind) {
	case STRPORT_CHAR:
		c = port->text[port->place];
		if (c != '\0')
			port->place++;
		return c;
	case STRPORT_FILE:
		return fgetc(port->fptr);
	default:
		exit(1);
	}
}
