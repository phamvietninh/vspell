#!/usr/bin/perl

# Get all content
while (<>) { chop;chop; $text .= "$_ "; }

$text =~ s/^.*<body[^>]*>//;
$text =~ s,</body.*$,,;
$text =~ s,<script[^>]*>[^<]*</script>,,g;
$text =~ s,<[^>]*>,\n,g;
$text =~ s,&nbsp;, ,g;
print $text;
