# Strings

s now has strings: a string is read as a single token.

There are two types of string: ' and "
* variables are not expanded inside single quote strings.
* all variables are expanded in double quote strings.

We support back slash escaping \ to allow you to insert quote
chars inside a string. (Also newline, tab, etc.)


## Examples

```
s$ echo "I am using the '$SHELL' shell"
I am using the '/bin/s' shell

s$ echo 'I am using the "$SHELL" shell'
I am using the "$SHELL" shell
```

You cannot escape the \$ to stop expansion inside a " string.
If you want to produce a $ inside a " string you need to do a
workaround like this:

```
s$ set DOL '$'
s$ echo "I am using the ${DOL}SHELL shell"
I am using the $SHELL shell
```
