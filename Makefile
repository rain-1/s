CC=gcc
CFLAGS=-std=c99 -Wall -Werror

SOURCES=tokenizer.c
OBJECTS=$(SOURCES:.c=.o)

all: t1

clean:
	rm -f *.o
	rm -f t1

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

t1: $(OBJECTS)
	gcc -o t1 $(OBJECTS) t1.c
