#!/bin/bash

#echo
#echo "Initialized" >&2

while read a; do
	echo "[" $a "]" >&2
	if echo $a|grep -q "^Input:"; then
		zenity --title "vspell wrapper" --entry --text="`echo $a|sed 's/^Input: *//'`                            " || exit
	fi
done
