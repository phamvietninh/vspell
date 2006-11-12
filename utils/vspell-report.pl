#!/usr/bin/perl

for $i (0..1) {
    for  $j (0..1) {
	$count{"L$i$j"} = 0;
	$count{"C$i$j"} = 0;
	$count{"T$i$j"} = 0;
	$count{"P$i$j"} = 0;
    }
}

while (<STDIN>) {
    $line ++ if m/^#Sentence: /;
    if (m/^(\d+) (\d+) (\d+) (\d+) (\d+) (\d+)$/) {
	$count{"L$2$3"} ++;
	$count{"C$2$3"} += $4;  # corrects
	$count{"T$2$3"} += $1;  # error total
	$count{"P$2$3"} += $5;  # positives
	$count{"A$2$3"} += $6;  # candidates
	$count += $4;
	$total += $1;
	$positives += $5;
	$candidates += $6;
    }
=comment
    for $i (0..1) {
	for  $j (0..1) {
	    $c = $count{"T$i$j"};
	    $c = 1 if $c == 0;
	    print $count{"C$i$j"}."/".$c." = ".($count{"C$i$j"} / $c)."\n";
	}
    }
=cut
}

#printf "C/T =\t\t%d/%d =\t%.02f\n", $count,$total, $count/$total;
#printf "(C+SC)/T =\t%d+%d/%d =\t%.02f\n",$count,$candidates,$total,($count+$candidates)/$total;
#printf "P/L =\t\t%d/%d =\t%.02f\n",$positives,$line,$positives/$line;
printf "(%d) C%.03f CA%.03f P%.03f E%.03f\n", $total, $count/$total, ($count+$candidates)/$total, $positives/$line, ($total-$count-$candidates)/$total;

if ($ARGV[1] == '--detail') {

for $i (0..1) {
    for  $j (0..1) {
    	next if ($count{"T$i$j"} == 0) or ($count{"C$i$j"} == 0);
	print "None\n" if $i == 0 and $j == 0;
	print "Syllable\n" if $i == 1 and $j == 0;
	print "Word\n" if $i == 0 and $j == 1;
	print "Both\n" if $i == 1 and $j == 1;
	$count{"T$i$j"} = 1 if $count{"T$i$j"} == 0;
	printf "C/T = %d/%d = %.02f\n", $count{"C$i$j"},$count{"T$i$j"},($count{"C$i$j"} / $count{"T$i$j"});
	printf "A/T = %d/%d = %.02f\n", $count{"A$i$j"},$count{"T$i$j"},$count{"A$i$j"}/$count{"T$i$j"};
	$count{"L$i$j"} = 1 if $count{"L$i$j"} == 0;
	printf "P/L = %d/%d = %.02f\n",$count{"P$i$j"},$count{"L$i$j"},$count{"P$i$j"}/$count{"L$i$j"};
    }
}
}
