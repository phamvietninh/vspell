#!/usr/bin/perl

while (<>) {
    while (m/(&[^;]*;)/g) {
	print $1."\n"; 
    }
}
