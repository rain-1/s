CC=gcc
CFLAGS=-std=c99 -Wall -Werror

SOURCES=tokenizer.c parser.c interpreter.c variables.c
OBJECTS=$(SOURCES:.c=.o)

all: t1 t2 t3 t4 t5 t6

clean:
	rm -f *.o
	rm -f t1 t2 t3 t4 t5 t6

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

t1: $(OBJECTS) t1.c
	gcc -o t1 $(OBJECTS) t1.c

t2: $(OBJECTS) t2.c
	gcc -o t2 $(OBJECTS) t2.c

t3: $(OBJECTS) t3.c
	gcc -o t3 $(OBJECTS) t3.c

t4: $(OBJECTS) t4.c
	gcc -o t4 $(OBJECTS) t4.c

t5: $(OBJECTS) t5.c
	gcc -o t5 $(OBJECTS) t5.c

t6: $(OBJECTS) t6.c
	gcc -o t6 $(OBJECTS) t6.c
