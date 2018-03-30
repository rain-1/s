#!/bin/bash
set -x
if [ ! -e linenoise/linenoise.c ]
then
	git submodule update --init
fi
cc -o s \
	-I linenoise \
	s.c \
	builtins.c     parser.c  tokenizer.c  variables.c \
	interpreter.c  region.c  stringport.c  util.c \
	linenoise/linenoise.c linenoise/encodings/utf8.c
cc -o supporting/glob \
	supporting/glob.c
cc -o supporting/redir-box \
	supporting/redir-box.c
( cd supporting ; ln -fs redir-box \< )
( cd supporting ; ln -fs redir-box \> )
( cd supporting ; ln -fs redir-box \>\> )
