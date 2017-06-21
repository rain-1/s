#!/bin/sh

for FILE in *.test
do
	EXPECT=${FILE%.test}.expect
	
	echo "running $FILE expecting $EXPECT."
	../s "$FILE" 2>&1 | diff -q "$EXPECT" - || (echo "fail" && exit)
done
