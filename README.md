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
S(1)                      BSD General Commands Manual                      S(1)

NAME
     s — command line interpreter

SYNOPSIS
     s [-dvh] [SCRIPT ...]

DESCRIPTION
     s has two main modes of operation: interactive shell and script processor.

     If the SCRIPT is provided it will execute that file with optional given
     arguments.  Alternatively you can provide commands via stdin.

     Otherwise if you start s in a terminal you will be given an interactive
     shell prompt like this:

             s$ echo hello world!
             hello world!

OPTIONS
     -d      Enable debug mode, reporting becomes more verbose showing file and
             line number of where the error or warning occurred.

     -v      Print version and copyright info and exit.

     -h      Print usage info and exit.

SYNTAX
     The lexical syntax of the shell language is very strictly tokenized based
     on spaces.

     Variables may occur inside tokens with ${FOO} syntax, the {}'s are
     optional for when variables need to be next to other strings:

             s$ set FOO bar
             s$ set avar value
             s$ echo $FOO ${avar}
             bar value
             s$ echo $FOO$avar
             barvalue
             s$ echo ${FOO}string$avar
             barstringvalue

     See the BUILTINS section below for info on setting variables.

     If found at the beginning of a token the ~ character expands to ${HOME} as
     a shortcut for the user's home directory.

     Single and double quotes are used to group the enclosed strings into one
     token.  Variables are only expanded inside double quotes. You can escape
     characters in quotes by using backslashes, allowing you to insert the same
     quote character inside a string, as well as inserting newline, tab, and
     spaces. Quotes can also be inserted inside the other quote types without
     escaping them.

             s$ echo "I am using the '$SHELL' shell"
             I am using the '/bin/s' shell
             s$ echo 'I am using the "$SHELL" shell'
             I am using the "$SHELL" shell

             s$ echo "you can use \ as well: \" \'"
             you can use  as well: " '

             $ echo 'person\tbalance\nRichard\t$500\nJohn\t$300'
             person  balance
             Richard $500
             John    $300

     Backticks can also be used to expand commands quoted inside to their out‐
     put:

             s$ rm `cat oldfiles.txt` # delete all files found in oldfiles.txt
             s$ set CFILES `ls *.c`   # make $CFILES equal to a list of all C files

     The # character, as seen above, starts a comment until the end of the
     line.

     The grammar is line based. Sequences of tokens are treated as commands and
     the operators |, &&, and || are parsed in order of tightest binding first.

     File redirection is achieve through the use of separate commands, thus
     pipes are needed:

             s$ echo "hello world" | > file.txt
             s$ echo "hello world again" | >> file.txt
             s$ cat | < file.txt
             hello world
             hello world again

     & at the end of a line can specify that a job should be run in the back‐
     ground.

     A backslash at the end of the line allows for commands to span multiple
     lines.

BUILTINS
     source SCRIPT [...]
             Open and execute commands in SCRIPT with the optional arguments
             provided.

     cd [DIR | -]
             Change current working directory to DIR.  If DIR is not supplied,
             change to $HOME instead.  If - is given switch directories to pre‐
             vious working directory, stored in $OLDPWD.

     set VAR VAL
             Set the environment variable given by VAR equal to VAL.

     unset VAR
             Remove environment variable VAR.

     exit [N]
             Exit s with code N, defaults to 0.

ENVIRONMENT
     SHELL   The full pathname to the shell.

     HOME    The home directory of the current user.

     PWD     Current working directory.

     OLDPWD  Previous working directory.

     PATH    A colon-separated list of directories in which s looks for com‐
             mands in.

AUTHORS
     rain-1 <rain1@openmailbox.org>

     Ed van Bruggen <edvb54@gmail.com>

LICENSE
     BSD 3 Clause

SEE ALSO
     execline(1)

     View source code and report bugs at: <https://github.com/rain-1/s>

S v0.0.0                          Jun 28, 2017                         S v0.0.0
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

