// -*- coding: viscii tab-width:2 mode: c++ -*-
#include <string>
#include <iostream>
#include <string.h>
#include "dictionary.h"
#include "syllable.h"
#include "spell.h"

/*
	VISCII character map.
<U1EB2>     /x02         LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
<U1EB4>     /x05         LATIN CAPITAL LETTER A WITH BREVE AND TILDE
<U1EAA>     /x06         LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
<U1EF6>     /x14         LATIN CAPITAL LETTER Y WITH HOOK ABOVE
<U1EF8>     /x19         LATIN CAPITAL LETTER Y WITH TILDE
<U1EF4>     /x1e         LATIN CAPITAL LETTER Y WITH DOT BELOW
<U1EA0>     /x80 �        LATIN CAPITAL LETTER A WITH DOT BELOW
<U1EAE>     /x81 �        LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
<U1EB0>     /x82 �        LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
<U1EB6>     /x83 �        LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
<U1EA4>     /x84 �        LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
<U1EA6>     /x85 �        LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
<U1EA8>     /x86 �        LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
<U1EAC>     /x87 �        LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
<U1EBC>     /x88 �        LATIN CAPITAL LETTER E WITH TILDE
<U1EB8>     /x89 �        LATIN CAPITAL LETTER E WITH DOT BELOW
<U1EBE>     /x8a �        LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
<U1EC0>     /x8b �        LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
<U1EC2>     /x8c �        LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
<U1EC4>     /x8d �        LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
<U1EC6>     /x8e �        LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
<U1ED0>     /x8f �        LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
<U1ED2>     /x90 �        LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
<U1ED4>     /x91 �        LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
<U1ED6>     /x92 �        LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
<U1ED8>     /x93 �        LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
<U1EE2>     /x94 �        LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
<U1EDA>     /x95 �        LATIN CAPITAL LETTER O WITH HORN AND ACUTE
<U1EDC>     /x96 �        LATIN CAPITAL LETTER O WITH HORN AND GRAVE
<U1EDE>     /x97 �        LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
<U1ECA>     /x98 �        LATIN CAPITAL LETTER I WITH DOT BELOW
<U1ECE>     /x99 �        LATIN CAPITAL LETTER O WITH HOOK ABOVE
<U1ECC>     /x9a �        LATIN CAPITAL LETTER O WITH DOT BELOW
<U1EC8>     /x9b �      LATIN CAPITAL LETTER I WITH HOOK ABOVE
<U1EE6>     /x9c �        LATIN CAPITAL LETTER U WITH HOOK ABOVE
<U0168>     /x9d �        LATIN CAPITAL LETTER U WITH TILDE
<U1EE4>     /x9e �        LATIN CAPITAL LETTER U WITH DOT BELOW
<U1EF2>     /x9f �        LATIN CAPITAL LETTER Y WITH GRAVE
<U00D5>     /xa0 �        LATIN CAPITAL LETTER O WITH TILDE
<U1EAF>     /xa1 �        LATIN SMALL LETTER A WITH BREVE AND ACUTE
<U1EB1>     /xa2 �        LATIN SMALL LETTER A WITH BREVE AND GRAVE
<U1EB7>     /xa3 �        LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
<U1EA5>     /xa4 �        LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
<U1EA7>     /xa5 �        LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
<U1EA9>     /xa6 �        LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
<U1EAD>     /xa7 �        LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
<U1EBD>     /xa8 �        LATIN SMALL LETTER E WITH TILDE
<U1EB9>     /xa9 �        LATIN SMALL LETTER E WITH DOT BELOW
<U1EBF>     /xaa �        LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
<U1EC1>     /xab �        LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
<U1EC3>     /xac �        LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
<U1EC5>     /xad �        LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
<U1EC7>     /xae �        LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
<U1ED1>     /xaf �        LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
<U1ED3>     /xb0 �        LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
<U1ED5>     /xb1 �        LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
<U1ED7>     /xb2 �        LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
<U1EE0>     /xb3 �        LATIN CAPITAL LETTER O WITH HORN AND TILDE
<U01A0>     /xb4 �        LATIN CAPITAL LETTER O WITH HORN
<U1ED9>     /xb5 �        LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
<U1EDD>     /xb6 �        LATIN SMALL LETTER O WITH HORN AND GRAVE
<U1EDF>     /xb7 �        LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
<U1ECB>     /xb8 �        LATIN SMALL LETTER I WITH DOT BELOW
<U1EF0>     /xb9 �        LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
<U1EE8>     /xba �        LATIN CAPITAL LETTER U WITH HORN AND ACUTE
<U1EEA>     /xbb �        LATIN CAPITAL LETTER U WITH HORN AND GRAVE
<U1EEC>     /xbc �        LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
<U01A1>     /xbd �        LATIN SMALL LETTER O WITH HORN
<U1EDB>     /xbe �        LATIN SMALL LETTER O WITH HORN AND ACUTE
<U01AF>     /xbf �        LATIN CAPITAL LETTER U WITH HORN
<U00C0>     /xc0 �        LATIN CAPITAL LETTER A WITH GRAVE
<U00C1>     /xc1 �        LATIN CAPITAL LETTER A WITH ACUTE
<U00C2>     /xc2 �        LATIN CAPITAL LETTER A WITH CIRCUMFLEX
<U00C3>     /xc3 �        LATIN CAPITAL LETTER A WITH TILDE
<U1EA2>     /xc4 �        LATIN CAPITAL LETTER A WITH HOOK ABOVE
<U0102>     /xc5 �        LATIN CAPITAL LETTER A WITH BREVE
<U1EB3>     /xc6 �        LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
<U1EB5>     /xc7 �        LATIN SMALL LETTER A WITH BREVE AND TILDE
<U00C8>     /xc8 �        LATIN CAPITAL LETTER E WITH GRAVE
<U00C9>     /xc9 �        LATIN CAPITAL LETTER E WITH ACUTE
<U00CA>     /xca �        LATIN CAPITAL LETTER E WITH CIRCUMFLEX
<U1EBA>     /xcb �        LATIN CAPITAL LETTER E WITH HOOK ABOVE
<U00CC>     /xcc �        LATIN CAPITAL LETTER I WITH GRAVE
<U00CD>     /xcd �        LATIN CAPITAL LETTER I WITH ACUTE
<U0128>     /xce �        LATIN CAPITAL LETTER I WITH TILDE
<U1EF3>     /xcf �        LATIN SMALL LETTER Y WITH GRAVE
<U0110>     /xd0 �        LATIN CAPITAL LETTER D WITH STROKE
<U1EE9>     /xd1 �        LATIN SMALL LETTER U WITH HORN AND ACUTE
<U00D2>     /xd2 �        LATIN CAPITAL LETTER O WITH GRAVE
<U00D3>     /xd3 �        LATIN CAPITAL LETTER O WITH ACUTE
<U00D4>     /xd4 �        LATIN CAPITAL LETTER O WITH CIRCUMFLEX
<U1EA1>     /xd5 �        LATIN SMALL LETTER A WITH DOT BELOW
<U1EF7>     /xd6 �        LATIN SMALL LETTER Y WITH HOOK ABOVE
<U1EEB>     /xd7 �        LATIN SMALL LETTER U WITH HORN AND GRAVE
<U1EED>     /xd8 �        LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
<U00D9>     /xd9 �        LATIN CAPITAL LETTER U WITH GRAVE
<U00DA>     /xda �        LATIN CAPITAL LETTER U WITH ACUTE
<U1EF9>     /xdb �        LATIN SMALL LETTER Y WITH TILDE
<U1EF5>     /xdc �        LATIN SMALL LETTER Y WITH DOT BELOW
<U00DD>     /xdd �        LATIN CAPITAL LETTER Y WITH ACUTE
<U1EE1>     /xde �        LATIN SMALL LETTER O WITH HORN AND TILDE
<U01B0>     /xdf �        LATIN SMALL LETTER U WITH HORN
<U00E0>     /xe0 �        LATIN SMALL LETTER A WITH GRAVE
<U00E1>     /xe1 �        LATIN SMALL LETTER A WITH ACUTE
<U00E2>     /xe2 �        LATIN SMALL LETTER A WITH CIRCUMFLEX
<U00E3>     /xe3 �        LATIN SMALL LETTER A WITH TILDE
<U1EA3>     /xe4 �        LATIN SMALL LETTER A WITH HOOK ABOVE
<U0103>     /xe5 �        LATIN SMALL LETTER A WITH BREVE
<U1EEF>     /xe6 �        LATIN SMALL LETTER U WITH HORN AND TILDE
<U1EAB>     /xe7 �        LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
<U00E8>     /xe8 �        LATIN SMALL LETTER E WITH GRAVE
<U00E9>     /xe9 �        LATIN SMALL LETTER E WITH ACUTE
<U00EA>     /xea �        LATIN SMALL LETTER E WITH CIRCUMFLEX
<U1EBB>     /xeb �        LATIN SMALL LETTER E WITH HOOK ABOVE
<U00EC>     /xec �        LATIN SMALL LETTER I WITH GRAVE
<U00ED>     /xed �        LATIN SMALL LETTER I WITH ACUTE
<U0129>     /xee �        LATIN SMALL LETTER I WITH TILDE
<U1EC9>     /xef �        LATIN SMALL LETTER I WITH HOOK ABOVE
<U0111>     /xf0 �        LATIN SMALL LETTER D WITH STROKE
<U1EF1>     /xf1 �        LATIN SMALL LETTER U WITH HORN AND DOT BELOW
<U00F2>     /xf2 �        LATIN SMALL LETTER O WITH GRAVE
<U00F3>     /xf3 �        LATIN SMALL LETTER O WITH ACUTE
<U00F4>     /xf4 �        LATIN SMALL LETTER O WITH CIRCUMFLEX
<U00F5>     /xf5 �        LATIN SMALL LETTER O WITH TILDE
<U1ECF>     /xf6 �        LATIN SMALL LETTER O WITH HOOK ABOVE
<U1ECD>     /xf7 �        LATIN SMALL LETTER O WITH DOT BELOW
<U1EE5>     /xf8 �        LATIN SMALL LETTER U WITH DOT BELOW
<U00F9>     /xf9 �        LATIN SMALL LETTER U WITH GRAVE
<U00FA>     /xfa �        LATIN SMALL LETTER U WITH ACUTE
<U0169>     /xfb �        LATIN SMALL LETTER U WITH TILDE
<U1EE7>     /xfc �        LATIN SMALL LETTER U WITH HOOK ABOVE
<U00FD>     /xfd �        LATIN SMALL LETTER Y WITH ACUTE
<U1EE3>     /xfe �        LATIN SMALL LETTER O WITH HORN AND DOT BELOW
<U1EEE>     /xff �        LATIN CAPITAL LETTER U WITH HORN AND TILDE
 */

using namespace std;
/*
	namespace Dictionary {
*/
static char *vowels[] = {
	"i�","y�","ia",
	"߽","�a","u�","ua",
	"a","�","�",
	"e","�",
	"o","�","�",
	"u","�","i","y",
	NULL
};

static char *first_consonants[] = {	// longest first
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
	"�",				// 18
	"g",				// 19
	"h",				// 20
	"l",				// 21
	"r",				// 22
	"s",				// 23
	"v",				// 24
	"x",				// 25
	NULL
};

static char *last_consonants[] = {	// longest first
	"nh","ng","ch",/*"gh","ph","th","tr","gi","kh",*/
	"c","m","n","p","t",/*"b","k","q","d","�","g","h","l","r","s","v","x",*/
	// these are semivowels, not consonants.
	"i","y","o","u",
	NULL
};

static char *padding_vowels[] = {
	"o","u",
	NULL
};

static char *diacritic_table[6] = {
	"a��e�io��u�y",
	"ᤡ������",
	"ॢ������",
	"����������",
	"��Ǩ��������",
	"է����������",
};

static char *case_table[2] = {
	"�����⤥��塢�ǣ��먩ꪫ�����������������������������������������", 
	"���À���Ł����ˈ�ʊ�����̛Θ�ҙ��ԏ�����������ٜ��������ݟ�", 
};

static char full_case_table[2][256];
static char cat_table[256];						// numeric,alpha...
static pair<char,unsigned char> full_diacritic_table[256];
#define CAT_ALPHA 1
#define CAT_DIGIT 2
#define CAT_SPACE 4
#define CAT_PUNCT 8
#define CAT_XDIGIT 16

static const char *syll_empty = "Empty";
static const char *syll_exist = "Exist";

static vector<confusion_set> confusion_sets;
std::vector<confusion_set>& get_confusion_sets()
{
	return confusion_sets;
}


string viet_tolower(const string &str);	// hack

bool syllable_init()
{
	int i,len = strlen(case_table[0]);

	for (i = 0;i < 256;i ++) {
		if (i < 128 && i > 32) {
			full_case_table[0][i] = tolower(i);
			full_case_table[1][i] = toupper(i);
		} else {
			full_case_table[0][i] = i;
			full_case_table[1][i] = i;
		}
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
		cat_table[(unsigned char)case_table[0][i]] = CAT_ALPHA;
		cat_table[(unsigned char)case_table[1][i]] = CAT_ALPHA;
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
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","y"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","u"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","o"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","u"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","m"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","p"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","u"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i�","u"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","u"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i�","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","m"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i�","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","p"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"o","i"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","i"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","i"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"o","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","m"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"o","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","p"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"o","ng"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"o","c"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","i"));
	confusion_sets.back().push_back(Syllable(NULL,"u","�","i"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","m"));
	confusion_sets.back().push_back(Syllable(NULL,"u","�","m"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","p"));
	confusion_sets.back().push_back(Syllable(NULL,"u","�","p"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","i"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"߽","i"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","u"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"߽","u"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","m"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"߽","m"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","p"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"߽","p"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"a","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"e","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","nh"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","nh"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i","ch"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i�","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i�","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i�","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"i�","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"u","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,"u","�","n"));
	confusion_sets.back().push_back(Syllable(NULL,"u","�","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,"u","�","t"));
	confusion_sets.back().push_back(Syllable(NULL,"u","�","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","ng"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"�","c"));

	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"߽","n"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"߽","ng"));
		
	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,"߽","t"));
	confusion_sets.back().push_back(Syllable(NULL,NULL,"߽","c"));
		
	confusion_sets.push_back(confusion_set());
	confusion_sets.back().push_back(Syllable(NULL,NULL,NULL,NULL,Syllable::Hook));
	confusion_sets.back().push_back(Syllable(NULL,NULL,NULL,NULL,Syllable::Tilde));
	//confusion_sets.back().push_back(Syllable(NULL,NULL,NULL,NULL,Syllable::Dot));
	return true;
}

Syllable::Syllable(const Syllable &sy)
{
	for (int i = 0;i < 5;i ++)
		components[i] = sy.components[i];
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
	string syllable(viet_tolower(string(str)));
	string ssyllable(str);

	// fisrt of all, extract diacritic.
	// because the syllable has been stardardized. just extract the diacritic.
	if (syllable[0] >= '0' && syllable[0] <= '5') {
		components[Diacritic] = syllable[0] - '0';
		syllable.erase(0,1);
		ssyllable.erase(0,1);
	}	else {
		components[Diacritic] = None;
		len = syllable.size();
		for (k = 0;k < len;k ++) {
			// look up into diacritic_table
			for (j = 1;j < 6;j ++) {
				char *pos = strchr(diacritic_table[j],syllable[k]);
				if (pos) {
					int ipos = pos - diacritic_table[j];
					syllable[k] = diacritic_table[0][ipos];	// remove diacritic
					if (viet_toupper(ssyllable[k]) == ssyllable[k])
						ssyllable[k] = viet_toupper(diacritic_table[0][ipos]);	// remove diacritic
					else
						ssyllable[k] = diacritic_table[0][ipos];	// remove diacritic
					components[Diacritic] = j;
					break;
				}
			}
		}
	}

	// there are 8 cases:
	int cases[8][4] = {
		{ 0,-1, 0, 0},							// F_VL
		{-1,-1, 0, 0},							// __VL
		{ 0,-1, 0,-1},							// F_V_
		{-1,-1, 0,-1},							// __V_
		{ 0, 0, 0, 0},							// FPVL
		{-1, 0, 0, 0},							// _PVL
		{ 0, 0, 0,-1},							// FPV_
		{-1, 0, 0,-1},							// _PV_
	};

	string saved_syllable = syllable;
	string saved_ssyllable = ssyllable;

	for (unsigned z = 0;z < 8;z ++) {
		bool ok = true;
		syllable = saved_syllable;
		ssyllable = saved_ssyllable;

		// parse from the pattern cases[z]
		for (unsigned zz = 0;ok && zz < 4;zz ++) {	// component
			components[zz] = -1;
			if (ok && cases[z][zz] == 0) {
				// get the first_consonant
				ok = false;
				len = syllable.size();
				char **p;
				switch (zz) {
				case First_Consonant: p = first_consonants; break;
				case Last_Consonant: p = last_consonants; break;
				case Padding_Vowel: p = padding_vowels; break;
				case Vowel: p = vowels; break;
				}
				for (i = 0;p[i] != 0; i++) {
					char *pattern = p[i];
					int pattern_len = strlen(pattern);
					
					if (len >= pattern_len && 	// equal is possible
							syllable.substr(0,pattern_len) == pattern) {
						//cerr << "Comp " << zz << " is <" << pattern << ">" << endl;
						components[zz] = i;
						scomponents[zz] = ssyllable.substr(0,pattern_len);
						syllable.erase(0,pattern_len);
						ssyllable.erase(0,pattern_len);
						ok = true;
						break;
					}
				}
			}
		}

		// some rules to prevent errors
		// the last consonant "i" is only followed by u+, u+o+, o+, a^, a, a(, u, uo^, o^, o
		// the last consonant "u" is only followed by i, ie^, e^, e, u+, u+o+, o+, a^, a, a(
		// padding vowels don't precede u, uo^, o^, o --> vowel is higher priority than padding vowel
		// fix some errors
		// C� c�c tr߶ng h�p sau: u� ua, ui, uy, oi, qu.
		// u�, ua, ui, oi ���c gi�i quy�t b�ng th� t� cases.
		if (ok && syllable.empty()) {
			// "q" always precedes 'u' (padding vowel)
			if (components[First_Consonant] != -1 &&
					!strcmp(first_consonants[components[First_Consonant]],"q") &&	// first consonant is 'q'
					(components[Padding_Vowel] == -1 ||
					 strcmp(padding_vowels[components[Padding_Vowel]],"u"))) // padding not exist or not 'u'
				ok = false;
			else if (components[Vowel] != -1 &&
							 !strcmp(vowels[components[Vowel]],"u") &&
							 components[Last_Consonant] != -1 &&
							 !strcmp(last_consonants[components[Last_Consonant]],"y"))
				ok = false;
		}

		if (ok && syllable.empty())
			return true;
		//else
		//cerr << "Case " << z << " failed" << endl;
	}

	return false;
}

std::ostream& operator << (std::ostream &os,const Syllable &sy)
{
	char **p;
	char *diacritics[] = {"_","'","`","?","~","."};
	for (int i = 0;i < 5;i ++) {
		if (sy.components[i] < 0) 
			os << "_";
		else {
			switch (i) {
			case Syllable::First_Consonant: p = first_consonants; break;
			case Syllable::Last_Consonant: p = last_consonants; break;
			case Syllable::Padding_Vowel: p = padding_vowels; break;
			case Syllable::Vowel: p = vowels; break;
			case Syllable::Diacritic: p = diacritics; break;
			}
			if (i != Syllable::Diacritic)
				os << sy.scomponents[i];
			else
				os << p[sy.components[i]];
		}
		os << " ";
	}
	return os;
}

string Syllable::to_str() const
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
			s += scomponents[i];			// no diacritic because i=0..3
			if (i == Vowel && components[Diacritic] != None) {
				int last;
				if (components[Last_Consonant] == -1)
					last = s.size() - strlen(scomponents[Vowel].c_str());
				else
					last = s.size()-1;
				int j = strchr(diacritic_table[0],viet_tolower(s[last])) - diacritic_table[0];
				if (viet_toupper(s[last]) == s[last])
					s[last] = viet_toupper(diacritic_table[components[Diacritic]][j]);
				else
					s[last] = diacritic_table[components[Diacritic]][j];
			}
		}
	}
	return s;
}


strid Syllable::to_id() const
{
	return get_sarch()[to_str()];
}

strid Syllable::to_std_id() const
{
	return get_sarch()[get_dic_syllable(to_str())];
}

/*
	void Syllable::standardize(std::string syllable)
	{
	// We just need to eliminate "�a", "�e", "�a"

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

string viet_tolower(const string &str)
{
	string s(str);
	int n = str.size();
	for (int i = 0;i < n;i ++)
		s[i] = viet_tolower(s[i]);
	return s;
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
		//p = full_diacritic_table[(unsigned char)str[i]];
		p = full_diacritic_table[(unsigned char)viet_tolower(str[i])];
		if (p.first > 0) {
			string ret = char(p.first+'0')+str;
			ret[i+1] = diacritic_table[0][p.second];
			if (viet_toupper(str[i]) == str[i])
				ret[i+1] = viet_toupper(ret[i+1]);
			return ret;
		}
	}
	return '0'+str;
}

string get_std_syllable(const string &str)
{
	string s = get_dic_syllable(str);
	if (get_sarch().in_dict(s) || get_sarch().in_dict(get_lowercased_syllable(s)))
		return s;
	else
		return string("0")+str;
}

string get_lowercased_syllable(const string &str)
{
	return viet_tolower(str);
}

bool operator < (const Syllable &s1,const Syllable &s2)
{
	for (int i = 0;i < 5;i ++) {
		if (s1.components[i] == s2.components[i])
			continue;
		if (s1.components[i] > s2.components[i])
			return false;
		if (s1.components[i] < s2.components[i])
			return true;
	}
	return false;
}

bool operator == (const Syllable &s1,const Syllable &s2)
{
	for (int i = 0;i < 5;i ++) {
		if (s1.components[i] == s2.components[i])
			continue;
		return false;
	}
	return true;
}

/*
	}
*/
