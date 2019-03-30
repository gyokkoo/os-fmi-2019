#!/bin/sh
for TEXT_WORD in `cat text.txt | tr '[:upper:]' '[:lower:]' | grep -o -E '\w+' | sort -u`
do
	TREAGREP_RESULT=`tre-agrep -i -s -99 -w $TEXT_WORD dic.txt -B | grep -v 0 | head -n 1`
	if [[ -n "$TREAGREP_RESULT" ]]; then
		echo $TEXT_WORD:"$TREAGREP_RESULT"
	fi
done
