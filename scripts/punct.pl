#!/usr/bin/perl

while (<>) {
    $text = $_;

    # entity
    $text =~ s/&\#([0-9]*);/ \#\1 /g;
    # foreign proper name
    while ($text =~ s/([A-ZĞ])\. ([A-ZĞ])/\1.\2/g) {}
    # double colon
    $chars = "a-záàäãÕâ¤¥¦ç§åå¡¢ÆÇ£éèë¨©êª«¬­®íìïî¸óòöõ÷ô¯°±²µ½¾¶·ŞşúùüûøßÑ×ØæñıÏÖÛÜğA-ZÁÀÄÃ€Â„…†‡ÅÅ‚ƒÉÈËˆ‰ÊŠ‹ŒÍÌ›Î˜ÓÒ™ šÔ‘’“´•–—³”ÚÙœ¿º»¼ÿ¹İŸĞ";
    $text =~ s/([$chars])([!~():,.;!?-])([^$chars])/\1\n\3/g;
    $text =~ s/([^$chars])([!~():,.;!?-])([$chars])/\1\n\3/g;
    $text =~ s/^([!~():,.;-])([$chars])/\n\2/g;
    # dot
    # $text =~ m/\. ([a-zğA-ZĞ])/ . \1/g;
    print $text;
}

# Local Variables:
# coding: viscii
# End:
