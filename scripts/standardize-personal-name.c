#!/usr/bin/perl

@vowels = ("iê","yê","ia",
	   "ß½","ßa","uô","ua",
	   "a","å","â",
	   "e","ê",
	   "o","ô","½",
	   "u","ß","i","y");

@first_consonants = ("ngh", "nh", "ng", "ch", "gh",	   
		     "ph", "th", "tr", "gi", "kh",	   
		     "c", "m", "n", "t", "b", "k",	   
		     "q", "d", "ğ", "g", "h", "l",
		     "r", "s", "v", "x");

@last_consonants = ("nh","ng","ch",
		    "c","m","n","p","t",
		    "i","y","o","u");

@padding_vowels = ( "o","u" );

@diacritic_table = ("aâåeêioô½ußy",
		    "á¤¡éªíó¯¾úÑı",
		    "à¥¢è«ìò°¶ù×Ï",
		    "ä¦Æë¬ïö±·üØÖ",
		    "ãçÇ¨­îõ²ŞûæÛ",
		    "Õ§£©®¸÷µşøñÜ");

@case_table =
    ("áàäãÕâ¤¥¦ç§å¡¢ÆÇ£éèë¨©êª«¬­®íìïî¸óòöõ÷ô¯°±²µ½¾¶·ŞşúùüûøßÑ×ØæñıÏÖÛÜğ", 
     "ÁÀÄÃ€Â„…†‡Å‚ƒÉÈËˆ‰ÊŠ‹ŒÍÌ›Î˜ÓÒ™ šÔ‘’“´•–—³”ÚÙœ¿º»¼ÿ¹İŸĞ");

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
		// We just need to eliminate "òa", "òe", "ùa"

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


# Local Variables:
# coding: viscii
# End:
