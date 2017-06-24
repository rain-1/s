#!/bin/sh

for FILE in scripts/*; do
	EXPECT=${FILE%.sh}.txt
	EXPECT=expect${EXPECT#scripts}

	echo "running $FILE expecting $EXPECT."
	../s "$FILE" 2>&1 | diff -q "$EXPECT" - || (echo "fail" && exit)
done
