# s version number
VERSION = 0.0.0

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# includes and libraries
INCS = -Iinclude -Ilinenoise
LIBS =

# flags
CPPFLAGS = -DVERSION=\"$(VERSION)\" -D_GNU_SOURCE
CFLAGS = -g -std=c99 -Wall ${INCS} ${CPPFLAGS}
LDFLAGS = -g ${LIBS}

# compiler and linker
CC = gcc
