# s

After the [Shellshock](https://en.wikipedia.org/wiki/Shellshock_(software_bug))
bug happened they fixed the parsing bug in bash that let people hijack
webservers by simply including `{ :; }`.

Fixing a bug after it's been found is good but it doesn't have to be the end.
Sometimes you think about the bigger picture and see if why it was ever a bug
in the first place.

I suspected that since bash syntax is very complex the parser is going to be a
lot of complicated code too. So I did some line counts of various shells to
what the situation is like:

                  .c       .h
    gnu bash:     138227,  13746
    zsh:          135589,   5698
    shivers scsh: 118475 (scheme)
    templeos:     119115
    mirbsd mksh:   29223,   2562
    debian dash:   16503,   2084
    freebsd sh:    15453,   1622
    es shell:       9017,   1436
    plan9 rc:       5989,    327
    execline:       3794,    117

The line count for templeos isn't just counting its shell. It's the linecount
of the entire system: The operating system kernel, the compiler, the shell, the
drawing program, the graphic user interface, the 3D flight simulator. **Bash,
zsh and scsh have more code in them than an entire operating system.**

Shell syntax being overcomplex and ideosyncratic is a long standing problem.
Other than shellshock, annoyances happen on a daily basis. Like the time a bug
in a bash script in steam rm -rf'd peoples home directory [Ran steam. It deleted everything](https://github.com/valvesoftware/steam-for-linux/issues/3671).
I've had makefiles break a few times because there's a space in my path. It
took these people four years to fix that [Build failure when builddir contains
spaces](https://github.com/nodejs/node-gyp/issues/65).

My idea to improve things is to use an extremely minimal shell with the
simplest syntax possible.

# The new command interpreter

So I designed a new shell that aims to be extremely simple, and implemened it
in a short number of lines of code. Right now it's 780 lines plus 1200 for the
linenoise library. (Technically it is a "command interpreter" not a shell since
shell implies it implements the POSIX shell standard).

Here is the man page for *s*:

```
S(1)                        General Commands Manual                       S(1)

NAME
     s – command line interpreter

SYNOPSIS
     s [script]

DESCRIPTION
     s has two main modes of operation: interactive shell and batch script
     processor.

     If the command line argument script is provided it will execute that
     file. This also allows you to use s as the shebang line in an executable.
     Alternatively you can provide commands via stdin.

     Otherwise if you start s in a terminal you will be given an interactive
     shell prompt like this:

             s$ echo hello world!
             hello world!

SYNTAX
     The lexical syntax of the shell language is very strictly tokenized based
     on spaces. Variables may occur inside tokens with ${FOO} syntax, the {}'s
     are optional.

     The # character starts a comment until the end of the line.

     A token always stays a single token there is no globs or bash style
     expansion. You will need to master xargs to use this shell.

GRAMMAR
     The grammar is line based. Sequences of tokens are treated as commands
     and the operators | , && and || are parsed in order of tightest binding
     first.

     At the end of a line you can specify that job should be run in the
     background with &


BUILTINS
     cd [directory]

     set variable value

     unset variable

     source filename

     If there is an error the builtins will cause the script to exit
     immediately, in interactive mode a warning is printed.
```

The important thing is what's missing: We don't have globs or "splatting" where
a variable `$FOO` turns into multiple command line arguments. One token stays
one token forever. This is a "no surprises" straightforward approach.

We don't even have redirection operators `>` in the shell language. They are
added as extra programs. So you write `grep foo bar | > out.txt`. `>` is just
another unix command. `<` is essentially `cat`. A `glob` program is also
provided along with *s*, so you write `glob rm *`.

The benefit of this extreme minimalism is that the implementation can be done
with a very logical pipeline of operations: tokenizing splits the input based
on spaces and expands the variables out, parsing looks for the operators and
builds an AST, interpreting is a simple recursive walk down the AST
implementing the various operators.

* [tokenizer.c](https://github.com/rain-1/s/blob/master/tokenizer.c)
* [variables.c](https://github.com/rain-1/s/blob/master/variables.c)
* [parser.c](https://github.com/rain-1/s/blob/master/parser.c)
* [interpreter.c](https://github.com/rain-1/s/blob/master/interpreter.c)
* [repo](https://github.com/rain-1/s)

# End

What do you think? I would be very interested in hearing peoples opinions
about *s* and the more general idea of stripping programs down to their
essential parts. Feel free to comment or make 'issues' on the repo if you have
any criticism, suggestions or just want to discuss.

# See also

* [execline](https://skarnet.org/software/execline/)

