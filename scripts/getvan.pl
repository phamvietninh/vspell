#!/usr/bin/perl

@consonants = ("ngh",  "nh",  "ng",  "ch",  "gh",  "ph",  "th",
	       "tr",  "gi",  "kh",  "c",  "m",  "n",  "t",  "b",
	       "k",  "q",  "d",  "®",  "g",  "h",  "l",  "r",  "s",
	       "v",  "x");

while (<>) {
    foreach $i (@consonants) {
	if (m/^$i(.*)/i) {
	    print $1."\n";
	    last;
	}
    }
}
# Local Variables:
# coding: vscii
# End:
