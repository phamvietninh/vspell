// -*- coding: viscii tab-width:2 mode: c++ -*-
#include <string>
#include <iostream>
#include <string.h>
#include "dictionary.h"
#include "syllable.h"

/*
	VISCII character map.
<U1EB2>     /x02         LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
<U1EB4>     /x05         LATIN CAPITAL LETTER A WITH BREVE AND TILDE
<U1EAA>     /x06         LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
<U1EF6>     /x14         LATIN CAPITAL LETTER Y WITH HOOK ABOVE
<U1EF8>     /x19         LATIN CAPITAL LETTER Y WITH TILDE
<U1EF4>     /x1e         LATIN CAPITAL LETTER Y WITH DOT BELOW
<U1EA0>     /x80 Ä        LATIN CAPITAL LETTER A WITH DOT BELOW
<U1EAE>     /x81 Å        LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
<U1EB0>     /x82 Ç        LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
<U1EB6>     /x83 É        LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
<U1EA4>     /x84 Ñ        LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
<U1EA6>     /x85 Ö        LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
<U1EA8>     /x86 Ü        LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
<U1EAC>     /x87 á        LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
<U1EBC>     /x88 à        LATIN CAPITAL LETTER E WITH TILDE
<U1EB8>     /x89 â        LATIN CAPITAL LETTER E WITH DOT BELOW
<U1EBE>     /x8a ä        LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
<U1EC0>     /x8b ã        LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
<U1EC2>     /x8c å        LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
<U1EC4>     /x8d ç        LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
<U1EC6>     /x8e é        LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
<U1ED0>     /x8f è        LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
<U1ED2>     /x90 ê        LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
<U1ED4>     /x91 ë        LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
<U1ED6>     /x92 í        LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
<U1ED8>     /x93 ì        LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
<U1EE2>     /x94 î        LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
<U1EDA>     /x95 ï        LATIN CAPITAL LETTER O WITH HORN AND ACUTE
<U1EDC>     /x96 ñ        LATIN CAPITAL LETTER O WITH HORN AND GRAVE
<U1EDE>     /x97 ó        LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
<U1ECA>     /x98 ò        LATIN CAPITAL LETTER I WITH DOT BELOW
<U1ECE>     /x99 ô        LATIN CAPITAL LETTER O WITH HOOK ABOVE
<U1ECC>     /x9a ö        LATIN CAPITAL LETTER O WITH DOT BELOW
<U1EC8>     /x9b õ      LATIN CAPITAL LETTER I WITH HOOK ABOVE
<U1EE6>     /x9c ú        LATIN CAPITAL LETTER U WITH HOOK ABOVE
<U0168>     /x9d ù        LATIN CAPITAL LETTER U WITH TILDE
<U1EE4>     /x9e û        LATIN CAPITAL LETTER U WITH DOT BELOW
<U1EF2>     /x9f ü        LATIN CAPITAL LETTER Y WITH GRAVE
<U00D5>     /xa0 †        LATIN CAPITAL LETTER O WITH TILDE
<U1EAF>     /xa1 °        LATIN SMALL LETTER A WITH BREVE AND ACUTE
<U1EB1>     /xa2 ¢        LATIN SMALL LETTER A WITH BREVE AND GRAVE
<U1EB7>     /xa3 £        LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
<U1EA5>     /xa4 §        LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
<U1EA7>     /xa5 •        LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
<U1EA9>     /xa6 ¶        LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
<U1EAD>     /xa7 ß        LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
<U1EBD>     /xa8 ®        LATIN SMALL LETTER E WITH TILDE
<U1EB9>     /xa9 ©        LATIN SMALL LETTER E WITH DOT BELOW
<U1EBF>     /xaa ™        LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
<U1EC1>     /xab ´        LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
<U1EC3>     /xac ¨        LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
<U1EC5>     /xad ≠        LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
<U1EC7>     /xae Æ        LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
<U1ED1>     /xaf Ø        LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
<U1ED3>     /xb0 ∞        LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
<U1ED5>     /xb1 ±        LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
<U1ED7>     /xb2 ≤        LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
<U1EE0>     /xb3 ≥        LATIN CAPITAL LETTER O WITH HORN AND TILDE
<U01A0>     /xb4 ¥        LATIN CAPITAL LETTER O WITH HORN
<U1ED9>     /xb5 µ        LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
<U1EDD>     /xb6 ∂        LATIN SMALL LETTER O WITH HORN AND GRAVE
<U1EDF>     /xb7 ∑        LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
<U1ECB>     /xb8 ∏        LATIN SMALL LETTER I WITH DOT BELOW
<U1EF0>     /xb9 π        LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
<U1EE8>     /xba ∫        LATIN CAPITAL LETTER U WITH HORN AND ACUTE
<U1EEA>     /xbb ª        LATIN CAPITAL LETTER U WITH HORN AND GRAVE
<U1EEC>     /xbc º        LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
<U01A1>     /xbd Ω        LATIN SMALL LETTER O WITH HORN
<U1EDB>     /xbe æ        LATIN SMALL LETTER O WITH HORN AND ACUTE
<U01AF>     /xbf ø        LATIN CAPITAL LETTER U WITH HORN
<U00C0>     /xc0 ¿        LATIN CAPITAL LETTER A WITH GRAVE
<U00C1>     /xc1 ¡        LATIN CAPITAL LETTER A WITH ACUTE
<U00C2>     /xc2 ¬        LATIN CAPITAL LETTER A WITH CIRCUMFLEX
<U00C3>     /xc3 √        LATIN CAPITAL LETTER A WITH TILDE
<U1EA2>     /xc4 ƒ        LATIN CAPITAL LETTER A WITH HOOK ABOVE
<U0102>     /xc5 ≈        LATIN CAPITAL LETTER A WITH BREVE
<U1EB3>     /xc6 ∆        LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
<U1EB5>     /xc7 «        LATIN SMALL LETTER A WITH BREVE AND TILDE
<U00C8>     /xc8 »        LATIN CAPITAL LETTER E WITH GRAVE
<U00C9>     /xc9 …        LATIN CAPITAL LETTER E WITH ACUTE
<U00CA>     /xca          LATIN CAPITAL LETTER E WITH CIRCUMFLEX
<U1EBA>     /xcb À        LATIN CAPITAL LETTER E WITH HOOK ABOVE
<U00CC>     /xcc Ã        LATIN CAPITAL LETTER I WITH GRAVE
<U00CD>     /xcd Õ        LATIN CAPITAL LETTER I WITH ACUTE
<U0128>     /xce Œ        LATIN CAPITAL LETTER I WITH TILDE
<U1EF3>     /xcf œ        LATIN SMALL LETTER Y WITH GRAVE
<U0110>     /xd0 –        LATIN CAPITAL LETTER D WITH STROKE
<U1EE9>     /xd1 —        LATIN SMALL LETTER U WITH HORN AND ACUTE
<U00D2>     /xd2 “        LATIN CAPITAL LETTER O WITH GRAVE
<U00D3>     /xd3 ”        LATIN CAPITAL LETTER O WITH ACUTE
<U00D4>     /xd4 ‘        LATIN CAPITAL LETTER O WITH CIRCUMFLEX
<U1EA1>     /xd5 ’        LATIN SMALL LETTER A WITH DOT BELOW
<U1EF7>     /xd6 ÷        LATIN SMALL LETTER Y WITH HOOK ABOVE
<U1EEB>     /xd7 ◊        LATIN SMALL LETTER U WITH HORN AND GRAVE
<U1EED>     /xd8 ÿ        LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
<U00D9>     /xd9 Ÿ        LATIN CAPITAL LETTER U WITH GRAVE
<U00DA>     /xda ⁄        LATIN CAPITAL LETTER U WITH ACUTE
<U1EF9>     /xdb €        LATIN SMALL LETTER Y WITH TILDE
<U1EF5>     /xdc ‹        LATIN SMALL LETTER Y WITH DOT BELOW
<U00DD>     /xdd ›        LATIN CAPITAL LETTER Y WITH ACUTE
<U1EE1>     /xde ﬁ        LATIN SMALL LETTER O WITH HORN AND TILDE
<U01B0>     /xdf ﬂ        LATIN SMALL LETTER U WITH HORN
<U00E0>     /xe0 ‡        LATIN SMALL LETTER A WITH GRAVE
<U00E1>     /xe1 ·        LATIN SMALL LETTER A WITH ACUTE
<U00E2>     /xe2 ‚        LATIN SMALL LETTER A WITH CIRCUMFLEX
<U00E3>     /xe3 „        LATIN SMALL LETTER A WITH TILDE
<U1EA3>     /xe4 ‰        LATIN SMALL LETTER A WITH HOOK ABOVE
<U0103>     /xe5 Â        LATIN SMALL LETTER A WITH BREVE
<U1EEF>     /xe6 Ê        LATIN SMALL LETTER U WITH HORN AND TILDE
<U1EAB>     /xe7 Á        LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
<U00E8>     /xe8 Ë        LATIN SMALL LETTER E WITH GRAVE
<U00E9>     /xe9 È        LATIN SMALL LETTER E WITH ACUTE
<U00EA>     /xea Í        LATIN SMALL LETTER E WITH CIRCUMFLEX
<U1EBB>     /xeb Î        LATIN SMALL LETTER E WITH HOOK ABOVE
<U00EC>     /xec Ï        LATIN SMALL LETTER I WITH GRAVE
<U00ED>     /xed Ì        LATIN SMALL LETTER I WITH ACUTE
<U0129>     /xee Ó        LATIN SMALL LETTER I WITH TILDE
<U1EC9>     /xef Ô        LATIN SMALL LETTER I WITH HOOK ABOVE
<U0111>     /xf0         LATIN SMALL LETTER D WITH STROKE
<U1EF1>     /xf1 Ò        LATIN SMALL LETTER U WITH HORN AND DOT BELOW
<U00F2>     /xf2 Ú        LATIN SMALL LETTER O WITH GRAVE
<U00F3>     /xf3 Û        LATIN SMALL LETTER O WITH ACUTE
<U00F4>     /xf4 Ù        LATIN SMALL LETTER O WITH CIRCUMFLEX
<U00F5>     /xf5 ı        LATIN SMALL LETTER O WITH TILDE
<U1ECF>     /xf6 ˆ        LATIN SMALL LETTER O WITH HOOK ABOVE
<U1ECD>     /xf7 ˜        LATIN SMALL LETTER O WITH DOT BELOW
<U1EE5>     /xf8 ¯        LATIN SMALL LETTER U WITH DOT BELOW
<U00F9>     /xf9 ˘        LATIN SMALL LETTER U WITH GRAVE
<U00FA>     /xfa ˙        LATIN SMALL LETTER U WITH ACUTE
<U0169>     /xfb ˚        LATIN SMALL LETTER U WITH TILDE
<U1EE7>     /xfc ¸        LATIN SMALL LETTER U WITH HOOK ABOVE
<U00FD>     /xfd ˝        LATIN SMALL LETTER Y WITH ACUTE
<U1EE3>     /xfe ˛        LATIN SMALL LETTER O WITH HORN AND DOT BELOW
<U1EEE>     /xff ˇ        LATIN CAPITAL LETTER U WITH HORN AND TILDE
 */

using namespace std;
/*
	namespace Dictionary {
*/
char *vowels[] = {
	"iÍ","yÍ","ia",
	"ﬂΩ","ﬂa","uÙ","ua",
	"a","Â","‚",
	"e","Í",
	"o","Ù","Ω",
	"u","ﬂ","i","y",
	NULL
};

char *first_consonants[] = {	// longest first
	"ngh",			// 0
	"nh",				// 1
	"ng",				// 2
	"ch",				// 3
	"gh",				// 4
	"ph",				// 5
	"th",				// 6
	"tr",				// 7
	"gi",				// 8
	"kh",				// 9
	"c",				// 10
	"m",				// 11
	"n"/*,"p"*/,			// 12
	"t",				// 13
	"b",				// 14
	"k",				// 15
	"q",				// 16
	"d",				// 17
	"",				// 18
	"g",				// 19
	"h",				// 20
	"l",				// 21
	"r",				// 22
	"s",				// 23
	"v",				// 24
	"x",				// 25
	NULL
};

char *last_consonants[] = {	// longest first
	"nh","ng","ch",/*"gh","ph","th","tr","gi","kh",*/
	"c","m","n","p","t",/*"b","k","q","d","","g","h","l","r","s","v","x",*/
	// these are semivowels, not consonants.
	"i","y","o","u",
	NULL
};

char *padding_vowels[] = {
	"o","u",
	NULL
};

char *diacritic_table[6] = {
	"a‚ÂeÍioÙΩuﬂy",
	"·§°È™ÌÛØæ˙—˝",
	"‡•¢Ë´ÏÚ∞∂˘◊œ",
	"‰¶∆Î¨Ôˆ±∑¸ÿ÷",
	"„Á«®≠Óı≤ﬁ˚Ê€",
	"’ß£©Æ∏˜µ˛¯Ò‹",
};

char *case_table[2] = {
	"·‡‰„’‚§•¶ÁßÂ°¢∆«£ÈËÎ®©Í™´¨≠ÆÌÏÔÓ∏ÛÚˆı˜ÙØ∞±≤µΩæ∂∑ﬁ˛˙˘¸˚¯ﬂ—◊ÿÊÒ˝œ÷€‹", 
	"¡¿ƒ√Ä¬ÑÖÜá≈ÅÇÉ…»Ààâ äãåçéÕÃõŒò”“ô†ö‘èêëíì¥ïñó≥î⁄Ÿúùûø∫ªºˇπ›ü–", 
};

char full_case_table[2][256];
char cat_table[256];						// numeric,alpha...
pair<char,unsigned char> full_diacritic_table[256];
#define CAT_ALPHA 1
#define CAT_DIGIT 2
#define CAT_SPACE 4
#define CAT_PUNCT 8
#define CAT_XDIGIT 16

const char *syll_empty = "Empty";
const char *syll_exist = "Exist";

typedef vector<Syllable> confusion_set;
vector<confusion_set> confusion_sets;

bool syllable_init()
{
	int i,len = strlen(case_table[0]);

	for (i = 0;i < 256;i ++) {
		full_case_table[0][i] = tolower(i);
		full_case_table[1][i] = toupper(i);
		cat_table[i] = 0;
		if (isalpha(i)) cat_table[i] |= CAT_ALPHA;
		if (isspace(i)) cat_table[i] |= CAT_SPACE;
		if (isdigit(i)) cat_table[i] |= CAT_DIGIT;
		if (isxdigit(i)) cat_table[i] |= CAT_XDIGIT;
		if (ispunct(i)) cat_table[i] |= CAT_PUNCT;
		full_diacritic_table[i] = make_pair(-1,0);
	}
	for (i = 0;i < len; i ++) {
		full_case_table[0][(unsigned char)case_table[1][i]] = case_table[0][i];
		full_case_table[1][(unsigned char)case_table[0][i]] = case_table[1][i];
		cat_table[(int)case_table[0][i]] = CAT_ALPHA;
		cat_table[(int)case_table[1][i]] = CAT_ALPHA;
	}

	for (i = 0;i < 6;i ++) {
		int j,n = strlen(diacritic_table[i]);
		for (j = 0;j < n;j ++)
			full_diacritic_table[(unsigned char)diacritic_table[i][j]] = make_pair(i,j);
	}

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable("c"));
	confusion_sets.back().push_back(Syllable("k"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back("g");
	confusion_sets.back().push_back("gh");

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back("ng");
	confusion_sets.back().push_back("ngh");

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back("ch");
	confusion_sets.back().push_back("tr");

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back("s");
	confusion_sets.back().push_back("x");

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back("v");
	confusion_sets.back().push_back("d");
	confusion_sets.back().push_back("gi");
	confusion_sets.back().push_back("r");

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(syll_empty,syll_exist));
	confusion_sets.back().push_back(Syllable("h",syll_exist));
	confusion_sets.back().push_back(Syllable("ng",syll_exist));
	confusion_sets.back().push_back(Syllable("q","u"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","i"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","y"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"‚","y"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","u"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","o"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"‚","u"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Â","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"‚","m"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Â","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"‚","p"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","u"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"iÍ","u"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Í","u"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"iÍ","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Í","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","m"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"iÍ","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Í","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","p"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"o","i"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Ù","i"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Ω","i"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"o","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Ù","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Ω","m"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"o","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Ù","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Ω","p"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"o","ng"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Ù","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"o","c"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Ù","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","i"));
	confusion_sets.back().push_back(Syllable(NULL,"u","Ù","i"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","m"));
	confusion_sets.back().push_back(Syllable(NULL,"u","Ù","m"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","p"));
	confusion_sets.back().push_back(Syllable(NULL,"u","Ù","p"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂ","i"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂΩ","i"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂ","u"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂΩ","u"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂ","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂΩ","m"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂ","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂΩ","p"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Â","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Â","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Â","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Â","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"‚","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"‚","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"‚","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"‚","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Í","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Í","nh"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Í","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Í","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","nh"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","ch"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"iÍ","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"iÍ","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"iÍ","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"iÍ","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Ω","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"Ω","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"æ","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"æ","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,"u","Ù","n"));
	confusion_sets.back().push_back(Syllable(NULL,"u","Ù","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,"u","Ù","t"));
	confusion_sets.back().push_back(Syllable(NULL,"u","Ù","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂ","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂ","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂ","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂ","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂΩ","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂΩ","ng"));
		
	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂΩ","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"ﬂΩ","c"));
		
	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,NULL,NULL,Syllable::Hook));
	confusion_sets.back().push_back(Syllable(NULL,NULL,NULL,NULL,Syllable::Tilde));
	confusion_sets.back().push_back(Syllable(NULL,NULL,NULL,NULL,Syllable::Dot));
	return true;
}

Syllable::Syllable(const char*  _first_consonant,
									 const char*  _padding_vowel,
									 const char*  _vowel,
									 const char*  _last_consonant,
									 int d)
{
	int __first_consonant = -1;
	int __padding_vowel = -1;
	int __vowel = -1;
	int __last_consonant = -1;
	//int __diacritic = -1;
	int i;

	if (_first_consonant == syll_exist)
		__first_consonant = -2;
	else if (_first_consonant == syll_empty)
		__first_consonant = -3;
	else if (_first_consonant != NULL)
		for (i = 0;first_consonants[i] != NULL;i ++)
			if (!strcmp(first_consonants[i],_first_consonant)) {
				__first_consonant = i;
				break;
			}

	if (_padding_vowel == syll_exist)
		__padding_vowel = -2;
	else if (_padding_vowel == syll_empty)
		__padding_vowel = -3;
	else if (_padding_vowel != NULL)
		for (i = 0;padding_vowels[i] != NULL;i ++) {
			if (!strcmp(padding_vowels[i],_padding_vowel)) {
				__padding_vowel = i;
				break;
			}
		}

	if (_vowel == syll_exist)
		__vowel = -2;
	else if (_vowel == syll_empty)
		__vowel = -3;
	else if (_vowel != NULL)
		for (i = 0;vowels[i] != NULL;i ++)
			if (!strcmp(vowels[i],_vowel)) {
				__vowel = i;
				break;
			}

	if (_last_consonant == syll_exist)
		__last_consonant = -2;
	else if (_last_consonant == syll_empty)
		__last_consonant = -3;
	else if (_last_consonant != NULL)
		for (i = 0;last_consonants[i] != NULL;i ++)
			if (!strcmp(last_consonants[i],_last_consonant)) {
				__last_consonant = i;
				break;
			}

	components[First_Consonant] = __first_consonant;
	components[Padding_Vowel] 	= __padding_vowel;
	components[Vowel] 		= __vowel;
	components[Last_Consonant] 	= __last_consonant;
	components[Diacritic] 	= d;
}

bool Syllable::match(const Syllable &sample)
{
	for (int i = 0;i < 5;i ++) {
		switch (components[i]) {
		case -1:break;		// it's alright
		case -2:			// exist
			if (sample.components[i] == -1)
				return false;
			break;
		case -3:			// empty
			if (sample.components[i] != -1)
				return false;
		default:
			if (components[i] != sample.components[i])
				return false;
		}
	}
	return true;
}

void Syllable::apply(const Syllable &sample,vector<Syllable> &output)
{
	int i,j;
	int iter[5];
	int limit[5];
	char **p;
	Syllable s = sample;
	bool run = false;

	// init phase
	for (i = 0;i < 5;i ++) {
		limit[i] = 1;
		iter[i] = 0;
		switch (components[i]) {
		case -1:break;
		case -2:
			switch (i) {
			case First_Consonant: p = first_consonants; break;
			case Last_Consonant: p = last_consonants; break;
			case Padding_Vowel: p = padding_vowels; break;
			case Vowel: p = vowels; break;
			}
			for (j = 0;p[j] != NULL;j ++);
			if (j > 1) {
				limit[i] = j;
				run = true;
			}
			break;
		case -3: s.components[i] = -1; break;
		default: s.components[i] = components[i]; break;
		}
	}

	if (!run) {
		output.push_back(s);
		return;
	}

	// generating phase
	int k,n = 5;
	i = 0;
	while (run) {
		if (i < n-1 && iter[i] < limit[i]) {
			i ++;
		} else {
			if (i == n-1 && iter[i] < limit[i]) {
				for (k = 0;k < n;k ++) {
					output.push_back(s);
					for (int kk = 0;kk < 5;kk ++)
						if (components[kk] == -2)
							output.back().components[kk] = iter[k];
				}
				iter[i] ++;
			} else {
				k = i;
				while (i >= 0 && iter[i] == limit[i])
					i --;
				if (i < 0)
					run = false;
				else {
					iter[i] ++;
					iter[k] = 0;
				}
			}
		}
	}
}

// we assumes str is a valid syllable ;)
bool Syllable::parse(const char *str)
{
	// Rule: there is always vowel. Others can be omitted.
	// [first_consonant] [padding_vowel] vowel [last_consonant]

	int i,j,k;
	//char **pattern;
	int len;
	string syllable(str);

	// fisrt of all, extract diacritic.
	// because the syllable has been stardardized. just extract the diacritic.
	if (syllable[0] >= '0' && syllable[0] <= '5') {
		components[Diacritic] = syllable[0] - '0';
		syllable.erase(0,1);
	}	else {
		components[Diacritic] = None;
		len = syllable.size();
		for (k = 0;k < len;k ++) {
			// look up into diacritic_table
			for (j = 1;j < 6;j ++) {
				char *pos = strchr(diacritic_table[j],syllable[k]);
				if (pos != NULL) {
					int ipos = pos - diacritic_table[j];
					syllable[k] = diacritic_table[0][ipos];	// remove diacritic
					components[Diacritic] = j;
					break;
				}
			}
		}
	}


	// first, get the last_consonant
	components[Last_Consonant] = -1;
	len = syllable.size();
	for (i = 0;last_consonants[i] != 0; i++) {
		char *pattern = last_consonants[i];
		int pattern_len = strlen(pattern);

		if (len > pattern_len &&
				syllable.substr(len-pattern_len) == pattern) {
			components[Last_Consonant] = i;
			syllable.erase(len-pattern_len);
			break;
		}
	}

	// then get the first_consonant
	components[First_Consonant] = -1;
	len = syllable.size();
	for (i = 0;first_consonants[i] != 0; i++) {
		char *pattern = first_consonants[i];
		int pattern_len = strlen(pattern);

		if (len >= pattern_len && 	// equal is possible
				syllable.substr(0,pattern_len) == pattern) {
			components[First_Consonant] = i;
			syllable.erase(0,pattern_len);
			break;
		}
	}

	// check if syllable is empty.
	// if it is, we can't use this last_consonant.
	// find the next last_consonant
	if (syllable.empty()) {
		if (components[Last_Consonant] == -1)
			return false;		// bad syllable

		syllable = last_consonants[components[Last_Consonant]];
		len = syllable.size();
		int start = components[Last_Consonant]+1;
		components[Last_Consonant] = -1;
		for (i = start;last_consonants[i] != 0; i++) {
			char *pattern = last_consonants[i];
			int pattern_len = strlen(pattern);

			if (len > pattern_len &&
					syllable.substr(len-pattern_len) == pattern) {
				components[Last_Consonant] = i;
				syllable.erase(len-pattern_len);
				break;
			}
		}
	}

	// get vowel
	components[Vowel] = -1;
	len = syllable.size();
	if (len == 0)
		return false;		// bad syllable
	for (i = 0;vowels[i] != 0; i++) {
		char *pattern = vowels[i];
		int pattern_len = strlen(pattern);

		if (len >= pattern_len && 	// equal is possible
				syllable.substr(len-pattern_len) == pattern) {
			components[Vowel] = i;
			syllable.erase(len-pattern_len);
			break;
		}
	}

	// the last is vowel
	components[Padding_Vowel] = -1;
	len = syllable.size();
	if (len == 0)
		return true;
	for (i = 0;padding_vowels[i] != 0; i++) {
		char *pattern = padding_vowels[i];
		//int pattern_len = strlen(pattern);

		if (syllable == pattern) {
			components[Padding_Vowel] = i;
			syllable = "";
			break;
		}
	}

	return components[Vowel] != -1 && syllable.empty();
}

void Syllable::print()
{
	char **p;
	char *diacritics[] = {"_","'","`","?","~","."};
	for (int i = 0;i < 5;i ++) {
		if (components[i] < 0) 
			cout << "_";
		else {
			switch (i) {
			case First_Consonant: p = first_consonants; break;
			case Last_Consonant: p = last_consonants; break;
			case Padding_Vowel: p = padding_vowels; break;
			case Vowel: p = vowels; break;
			case Diacritic: p = diacritics; break;
			}
			cout << p[components[i]];
		}
		cout << " ";
	}
}

string Syllable::to_str()
{
	string s;
	char **p;
	for (int i = 0;i < 4;i ++) {
		if (components[i] >= 0) {
			switch (i) {
			case First_Consonant: p = first_consonants; break;
			case Last_Consonant: p = last_consonants; break;
			case Padding_Vowel: p = padding_vowels; break;
			case Vowel: p = vowels; break;
			}
			s += p[components[i]];
			if (i == Vowel && components[Diacritic] != None) {
				int last = s.size()-1;
				int j = strchr(diacritic_table[0],s[last]) - diacritic_table[0];
				s[last] = diacritic_table[components[Diacritic]][j];
			}
		}
	}
	return s;
}


strid Syllable::to_id()
{
	return sarch[to_str()];
}

strid Syllable::to_std_id()
{
	return sarch[get_dic_syllable(to_str())];
}

/*
	void Syllable::standardize(std::string syllable)
	{
	// We just need to eliminate "Úa", "Úe", "˘a"

	// first, cut the first consonant off
	int start = 0;
	int len = syllable.size();
	for (i = 0;first_consonants[i] != 0; i++) {
	char *pattern = *first_consonants[i];
	int pattern_len = strlen(pattern);

	if (len > pattern_len &&
	syllable.compare(0,pattern_len,pattern) == 0) {
	start = i;
	break;
	}
	}

	}
*/

int viet_toupper(int ch)	// must be sure ch is a character
{
	return full_case_table[1][(unsigned char)(char)ch];
}

int viet_tolower(int ch)	// must be sure ch is a character
{
	return full_case_table[0][(unsigned char)(char)ch];
}

bool viet_isupper(int ch)
{
	return viet_isalpha(ch) && full_case_table[1][ch] == ch;
}

bool viet_islower(int ch)
{
	return viet_isalpha(ch) && full_case_table[0][ch] == ch;
}

bool viet_isalpha(int ch)
{
	return cat_table[ch] & CAT_ALPHA;
}

bool viet_isdigit(int ch)
{
	return cat_table[ch] & CAT_DIGIT;
}

bool viet_isxdigit(int ch)
{
	return cat_table[ch] & CAT_XDIGIT;
}

bool viet_isspace(int ch)
{
	return cat_table[ch] & CAT_SPACE;
}

bool viet_ispunct(int ch)
{
	return cat_table[ch] & CAT_PUNCT;
}

string get_dic_syllable(const string &str)
{
	uint i,n = str.size();

	for (i = 0;i < n;i ++) {
		pair<char,unsigned char> p;
		p = full_diacritic_table[(unsigned char)str[i]];
		if (p.first > 0) {
			string ret = char(p.first+'0')+str;
			ret[i+1] = diacritic_table[0][p.second];
			return ret;
		}
	}
	return '0'+str;
}

string get_std_syllable(const string &str)
{
	string s = get_dic_syllable(str);
	if (sarch.in_dict(s))
		return s;
	else
		return str;
}

string get_lowercased_syllable(const string &str)
{
	string s(str);
	transform(s.begin(),s.end(),s.begin(),viet_tolower);
	return s;
}
/*
	}
*/
