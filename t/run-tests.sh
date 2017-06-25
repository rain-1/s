#!/bin/bash

total=0
pass=0

for file in scripts/*; do
	expect=${file%.sh}.txt
	expect=expect${expect#scripts}

	echo -n "testing $file ... "
	if [[ $(../s "$file" 2>&1 | diff -q "$expect" - ) ]]; then
		echo fail
	else
		echo ok
		((pass++))
	fi
	((total++))
done

echo "$pass/$total tests passed"

[[ $pass == $total ]] && exit 0
exit 1
