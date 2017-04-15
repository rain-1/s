CC=gcc
CFLAGS=-std=c99 -D_GNU_SOURCE -Wall -Werror

SOURCES=tokenizer.c parser.c interpreter.c variables.c builtins.c region.c stringport.c linenoise/linenoise.c
OBJECTS=$(SOURCES:.c=.o)

all: s

clean:
	rm -f *.o
	rm -f t6
	rm -f s
	rm -f supporting/\<
	rm -f supporting/\>
	rm -f supporting/\>\>
	rm -f supporting/redir-box
	rm -f supporting/glob

%.o: %.c
	$(CC) -I linenoise -c $(CFLAGS) $< -o $@

#t6: $(OBJECTS) t6.c
#	$(CC) -o t6 $(OBJECTS) t6.c

s: linenoise $(OBJECTS) s.c
	$(CC) -o s $(OBJECTS) s.c

supporting/redir-box: supporting/redir-box.c
	$(CC) -o supporting/redir-box supporting/redir-box.c

supporting/glob: supporting/glob.c
	$(CC) -o supporting/glob supporting/glob.c

supporting: supporting/redir-box supporting/glob
	rm -f supporting/\<
	rm -f supporting/\>
	rm -f supporting/\>\>
	cd supporting ; ln -s redir-box \<
	cd supporting ; ln -s redir-box \>
	cd supporting ; ln -s redir-box \>\>

linenoise:
	@echo "This project requires the linenoise library!"
	@echo "git clone https://github.com/antirez/linenoise.git"
	@exit 1

install: s supporting
	mkdir -p out
	cp s out/
	cp supporting/redir-box out/
	cp 'supporting/<' out/
	cp 'supporting/>' out/
	cp 'supporting/>>' out/
	cp 'supporting/glob' out/
