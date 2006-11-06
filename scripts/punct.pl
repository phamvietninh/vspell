#!/usr/bin/perl

while (<>) {
    $text = $_;

    # entity
    $text =~ s/&\#([0-9]*);/ \#\1 /g;
    # foreign proper name
    while ($text =~ s/([A-Z�])\. ([A-Z�])/\1.\2/g) {}
    # double colon
    $chars = "a-z�����⤥���塢�ǣ��먩ꪫ�����������������������������������������A-Z���À����Ł����ˈ�ʊ�����̛Θ�ҙ��ԏ�����������ٜ��������ݟ�";
    $text =~ s/([$chars])([!~():,.;!?-])([^$chars])/\1\n\3/g;
    $text =~ s/([^$chars])([!~():,.;!?-])([$chars])/\1\n\3/g;
    $text =~ s/^([!~():,.;-])([$chars])/\n\2/g;
    # dot
    # $text =~ m/\. ([a-z�A-Z�])/ . \1/g;
    print $text;
}

# Local Variables:
# coding: viscii
# End:
