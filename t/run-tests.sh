#!/bin/sh

for FILE in test/*.test
do
	EXPECT=${FILE%.test}.expect
	EXPECT=expect${EXPECT#test}
	
	echo "running $FILE expecting $EXPECT."
	../s "$FILE" 2>&1 | diff -q "$EXPECT" - || (echo "fail" && exit)
done
