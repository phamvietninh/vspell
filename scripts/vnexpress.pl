#!/usr/bin/perl

while (<>) { 
=comment
    chop;
    chop;
    $start = 1 if m,^<td>$,;
    $start = 0 if m,^</td>$,;
    $text .= "$_\n" if $start == 1;
=cut
    chop;chop;$text .= "$_ ";
}
$text =~ s,.*<P class=Title>,,i;
$start = 1;
while ($text =~ m,([^<]*)<([^>]*)>,gi) {
    $oldstart = $start;
    $content = $1;
    $tag = $2;
    if ($tag eq "/P") {
	$start = 0;
	print "\n";
    }
    $start = 1 if $tag =~ m/^P class/i;
    print "$content<${tag}>" if $oldstart == 1;
#    print "= $tag\n";
}

