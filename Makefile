# s - command line interpreter
# See LICENSE file for copyright and license details.

include config.mk

EXE = s
SUP = supporting/\< supporting/\> supporting/\>\> supporting/redir-box supporting/glob
SRC = $(wildcard *.c) linenoise/linenoise.c
OBJ = $(SRC:.c=.o)

all: options $(EXE)

options:
	@echo $(EXE) build options:
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"

.o:
	@echo $(LD) $@
	@$(LD) -o $@ $< $(LDFLAGS)

.c.o:
	@echo $(CC) $<
	@$(CC) -c -o $@ $< $(CFLAGS)

supporting/redir-box: supporting/redir-box.c
	@echo $(CC) $<
	@$(CC) -o supporting/redir-box supporting/redir-box.c

supporting/glob: supporting/glob.c
	@echo $(CC) $<
	@$(CC) -o supporting/glob supporting/glob.c

supporting: supporting/redir-box supporting/glob
	@echo creating redir symlinks
	@cd supporting; ln -fs redir-box \<
	@cd supporting; ln -fs redir-box \>
	@cd supporting; ln -fs redir-box \>\>

linenoise:
	@echo "This project requires the linenoise library!"
	@echo "git clone https://github.com/antirez/linenoise.git"
	@exit 1

${OBJ}: config.mk

$(EXE): linenoise $(OBJ) supporting
	@echo $(CC) -o $@
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	@echo -n cleaning ...
	@rm -f $(OBJ) $(EXE)
	@rm -f supporting/*.o
	@rm -f $(SUP)
	@echo \ done

install: all
	@echo -n installing $(EXE) to $(DESTDIR)$(PREFIX)/bin ...
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f $(EXE) $(DESTDIR)$(PREFIX)/bin
	@chmod 755 $(DESTDIR)$(PREFIX)/bin/$(EXE)
	@echo \ done
	@echo -n installing supporting files to $(DESTDIR)$(PREFIX)/bin ...
	@cp -f $(SUP) $(DESTDIR)$(PREFIX)/bin
	@echo \ done
	@echo -n installing manual page to $(DESTDIR)$(MANPREFIX)/man1 ...
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@sed "s/VERSION/$(VERSION)/g" < $(EXE).1 > $(DESTDIR)$(MANPREFIX)/man1/$(EXE).1
	@chmod 644 $(DESTDIR)$(MANPREFIX)/man1/$(EXE).1
	@echo \ done

uninstall:
	@echo -n removing executable file from $(DESTDIR)$(PREFIX)/bin ...
	@rm -f $(DESTDIR)$(PREFIX)/bin/$(EXE)
	@echo \ done
	@echo -n removing supporting files from $(DESTDIR)$(PREFIX)/bin ...
	@rm -f $(DESTDIR)$(PREFIX)/bin/$(SUP)
	@echo \ done
	@echo -n removing manual page from $(DESTDIR)$(MANPREFIX)/man1 ...
	@rm -f $(DESTDIR)$(MANPREFIX)/man1/$(EXE).1
	@echo \ done

test:
	cd t ; ./run-tests.sh

.PHONY: all options clean install uninstall test
