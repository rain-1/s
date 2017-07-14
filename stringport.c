/* see LICENSE file for copyright and license details */
#include <stdio.h>
#include <stdlib.h>

#include "stringport.h"
#include "reporting.h"

int
port_peek(string_port *port)
{
	int c;
	switch (port->kind) {
	case STRPORT_CHAR:
		return port->text[port->place];
	case STRPORT_FILE:
		c = fgetc(port->fptr);
		ungetc(c, port->fptr);
		return c;
	}
	reporterr("port set to wrong kind");
}

int
port_eof(string_port *port)
{
	switch (port->kind) {
	case STRPORT_CHAR:
		return port->text[port->place] == '\0';
	case STRPORT_FILE:
		return feof(port->fptr);
	}
	reporterr("port set to wrong kind");
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
	}
	reporterr("port set to wrong kind");
}
