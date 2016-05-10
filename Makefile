CC=gcc
CFLAGS=-std=c99 -D_GNU_SOURCE -Wall -Werror

SOURCES=tokenizer.c parser.c interpreter.c variables.c builtins.c region.c
OBJECTS=$(SOURCES:.c=.o)

all: t6 s

clean:
	rm -f *.o
	rm -f t6
	rm -f s
	rm -f supporting/\<
	rm -f supporting/\>
	rm -f supporting/\>\>
	rm -f supporting/redir-box

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

t6: $(OBJECTS) t6.c
	$(CC) -o t6 $(OBJECTS) t6.c

s: $(OBJECTS) s.c
	$(CC) -o s $(OBJECTS) s.c

supporting/redir-box: supporting/redir-box.c
	$(CC) -o supporting/redir-box supporting/redir-box.c

redir-box: supporting/redir-box
	rm -f supporting/\<
	rm -f supporting/\>
	rm -f supporting/\>\>
	cd supporting ; ln -s redir-box \<
	cd supporting ; ln -s redir-box \>
	cd supporting ; ln -s redir-box \>\>
