// -*- tab-width:2 coding: viscii mode: c++ -*-
#include <string>
#include <iostream>
#include <string.h>
#include "dictionary.h"

using namespace std;

namespace Dictionary {

	char *vowels[] = {
		"iš¢ê","yš¢ê","ia",
		"š¢ßš¢½","š¢ßa","uš¢ô","ua",
		"a","š¢å","š¢â",
		"e","š¢ê",
		"o","š¢ô","š¢½",
		"u","š¢ß","i","y",
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
		"š¢ð",				// 18
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
		"c","m","n","p","t",/*"b","k","q","d","š¢ð","g","h","l","r","s","v","x",*/
		// these are semivowels, not consonants.
		"i","y","o","u",
		NULL
	};

	char *padding_vowels[] = {
		"o","u",
		NULL
	};

	char *diacritic_table[6] = {
		"aš¢âš¢åeš¢êioš¢ôš¢½uš¢ßy",
		"š¢áš¢¤š¢¡š¢éš¢ªš¢íš¢óš¢¯š¢¾š¢úš¢Ñš¢ý",
		"š¢àš¢¥š¢¢š¢èš¢«š¢ìš¢òš¢°š¢¶š¢ùš¢×š¢Ï",
		"š¢äš¢¦š¢Æš¢ëš¢¬š¢ïš¢öš¢±š¢·š¢üš¢Øš¢Ö",
		"š¢ãš¢çš¢Çš¢¨š¢­š¢îš¢õš¢²š¢Þš¢ûš¢æš¢Û",
		"š¢Õš¢§š¢£š¢©š¢®š¢¸š¢÷š¢µš¢þš¢øš¢ñš¢Ü",
	};

	char *case_table[2] = {
		"š¢áš¢àš¢äš¢ãš¢Õš¢âš¢¤š¢¥š¢¦š¢çš¢§š¢åš¢¡š¢¢š¢Æš¢Çš¢£š¢éš¢èš¢ëš¢¨š¢©š¢êš¢ªš¢«š¢¬š¢­š¢®š¢íš¢ìš¢ïš¢îš¢¸š¢óš¢òš¢öš¢õš¢÷š¢ôš¢¯š¢°š¢±š¢²š¢µš¢½š¢¾š¢¶š¢·š¢Þš¢þš¢úš¢ùš¢üš¢ûš¢øš¢ßš¢Ñš¢×š¢Øš¢æš¢ñš¢ýš¢Ïš¢Öš¢Ûš¢Üš¢ð", 
		"š£áš£àš£äš£ãš£Õš£âš£¤š£¥š£¦š£çš£§š£åš£¡š£¢š£Æš£Çš££š£éš£èš£ëš£¨š£©š£êš£ªš£«š£¬š£­š£®š£íš£ìš£ïš£îš£¸š£óš£òš£öš£õš£÷š£ôš£¯š£°š£±š£²š£µš£½š£¾š£¶š£·š£Þš£þš£úš£ùš£üš£ûš£øš£ßš£Ñš£×š£Øš£æš£ñš£ýš£Ïš£Öš£Ûš£Üš£ð", 
	};

	char full_case_table[2][256];

	const char *syll_empty = "Empty";
	const char *syll_exist = "Exist";

	typedef vector<Syllable> confusion_set;
	vector<confusion_set> confusion_sets;

	bool syllable_init()
	{
		int i,len = strlen(case_table[0]);

		for (i = 0;i < 256;i ++) {
			full_case_table[0][i] = isalpha(i) ? tolower(i) : 0;
			full_case_table[1][i] = isalpha(i) ? toupper(i) : 0;
		}
		for (i = 0;i < len; i ++) {
			full_case_table[0][(unsigned char)case_table[1][i]] = case_table[0][i];
			full_case_table[1][(unsigned char)case_table[0][i]] = case_table[1][i];
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
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢â","y"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"a","u"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"a","o"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢â","u"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢å","m"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢â","m"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢å","p"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢â","p"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"i","u"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"iš¢ê","u"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ê","u"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"i","m"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"iš¢ê","m"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ê","m"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"e","m"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"i","p"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"iš¢ê","p"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ê","p"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"e","p"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"o","i"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ô","i"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢½","i"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"o","m"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ô","m"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢½","m"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"o","p"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ô","p"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢½","p"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"o","ng"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ô","ng"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"o","c"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ô","c"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"u","i"));
		confusion_sets.back().push_back(Syllable(NULL,"u","š¢ô","i"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"u","m"));
		confusion_sets.back().push_back(Syllable(NULL,"u","š¢ô","m"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"u","p"));
		confusion_sets.back().push_back(Syllable(NULL,"u","š¢ô","p"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ß","i"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ßš¢½","i"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ß","u"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ßš¢½","u"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ß","m"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ßš¢½","m"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ß","p"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ßš¢½","p"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"a","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"a","ng"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"a","t"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"a","c"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢å","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢å","ng"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢å","t"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢å","c"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢â","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢â","ng"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢â","t"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢â","c"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"e","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"e","ng"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"e","t"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"e","c"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ê","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ê","nh"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ê","t"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ê","c"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"i","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"i","nh"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"i","t"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"i","ch"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"iš¢ê","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"iš¢ê","ng"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"iš¢ê","t"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"iš¢ê","c"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢½","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢½","ng"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢¾","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢¾","ng"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"u","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"u","ng"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"u","t"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"u","c"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,"u","š¢ô","n"));
		confusion_sets.back().push_back(Syllable(NULL,"u","š¢ô","ng"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,"u","š¢ô","t"));
		confusion_sets.back().push_back(Syllable(NULL,"u","š¢ô","c"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ß","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ß","ng"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ß","t"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ß","c"));

		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ßš¢½","n"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ßš¢½","ng"));
		
		confusion_sets.push_back(confusion_set());
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ßš¢½","t"));
		confusion_sets.back().push_back(Syllable(NULL,NULL,"š¢ßš¢½","c"));
		
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
		int __diacritic = -1;
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
			for (i = 0;padding_vowels[i] != NULL;i ++)
	if (!strcmp(padding_vowels[i],_padding_vowel)) {
		__padding_vowel = i;
		break;
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
		char **pattern;
		int len;
		string syllable(str);

		// fisrt of all, extract diacritic.
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
			int pattern_len = strlen(pattern);

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
		for (int i = 0;i < 4;i ++) {
			if (components[i] < 0) 
	cerr << "_";
			else {
	switch (i) {
	case First_Consonant: p = first_consonants; break;
	case Last_Consonant: p = last_consonants; break;
	case Padding_Vowel: p = padding_vowels; break;
	case Vowel: p = vowels; break;
	}
	cerr << p[components[i]];
			}
			cerr << " ";
		}
	}

	strid Syllable::to_id()
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
		return sarch[s];
	}

	/*
		void Syllable::standardize(std::string syllable)
		{
		// We just need to eliminate "š¢òa", "š¢òe", "š¢ùa"

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
		return full_case_table[1][ch] == ch;
	}

	bool viet_islower(int ch)
	{
		return full_case_table[0][ch] == ch;
	}

}
