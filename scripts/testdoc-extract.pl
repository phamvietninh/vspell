#!/usr/bin/perl


while (<>) {
    chop($_);
    $s = "";
    %ss = {};
    while (m/{([^}]*)}/g) {
	@s = split(/,/,lc($1));
	$sss = join('',sort(@s));
	#print "\n>>$sss<<<\n";
	if (!defined($ss{$sss})) {
	    $s .= " (".join(',',@s).')';
	    $ss{$sss} = "true";
	    #print ">>>>>>>>>>ADD";
	}
    }
    $_ =~ s/{([^},]*),[^}]*}/\\textbf{\1}/g;
    print '\noindent '.$_.'\hfill'.$s.'\par'."\n";
}
