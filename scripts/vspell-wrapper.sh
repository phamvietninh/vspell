#!/bin/bash
[ -p vspell-in ] || (rm -f vspell-in;mkfifo vspell-in)
[ -p vspell-out ] || (rm -f vspell-out;mkfifo vspell-out)
./vspell $1 $2 < vspell-in 2>vspell-out&
exec zenity.sh  > vspell-in < vspell-out
