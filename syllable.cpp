// -*- coding: viscii mode: c++ -*-
#include <string>
#include <iostream>
#include <string.h>
#include "dictionary.h"

using namespace std;

namespace Dictionary {

  char *vowels[] = {
    "iê","yê","ia",
    "ß½","ßa","uô","ua",
    "a","å","â",
    "e","ê",
    "o","ô","½",
    "u","ß","i","y",
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
    "ğ",				// 18
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
    "c","m","n","p","t",/*"b","k","q","d","ğ","g","h","l","r","s","v","x",*/
    // these are semivowels, not consonants.
    "i","y","o","u",
    NULL
  };

  char *padding_vowels[] = {
    "o","u",
    NULL
  };

  char *diacritic_table[6] = {
    "aâåeêioô½ußy",
    "á¤¡éªíó¯¾úÑı",
    "à¥¢è«ìò°¶ù×Ï",
    "ä¦Æë¬ïö±·üØÖ",
    "ãçÇ¨­îõ²ŞûæÛ",
    "Õ§£©®¸÷µşøñÜ",
  };

  char *case_table[2] = {
    "áàäãÕâ¤¥¦ç§å¡¢ÆÇ£éèë¨©êª«¬­®íìïî¸óòöõ÷ô¯°±²µ½¾¶·ŞşúùüûøßÑ×ØæñıÏÖÛÜğ", 
    "ÁÀÄÃ€Â„…†‡Å‚ƒÉÈËˆ‰ÊŠ‹ŒÍÌ›Î˜ÓÒ™ šÔ‘’“´•–—³”ÚÙœ¿º»¼ÿ¹İŸĞ", 
  };

  char full_case_table[2][256];

  bool syllable_init()
  {
    int i,len = strlen(case_table[0]);
    for (i = 0;i < 256;i ++) {
      full_case_table[0][i] = i >= 'A' && i <= 'Z' ? i+32 : i;
      full_case_table[1][i] = i >= 'a' && i <= 'z' ? i-32 : i;
    }
    for (i = 0;i < len; i ++) {
      full_case_table[0][(unsigned char)case_table[1][i]] = case_table[0][i];
      full_case_table[1][(unsigned char)case_table[0][i]] = case_table[1][i];
    }
    return true;
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
    len = syllable.size();
    for (k = 0;k < len;k ++) {
      // look up into diacritic_table
      for (j = 1;j < 6;j ++) {
	char *pos = strchr(diacritic_table[j],syllable[k]);
	if (pos != NULL) {
	  int ipos = pos - diacritic_table[j];
	  syllable[k] = diacritic_table[0][ipos];	// remove diacritic
	  diacritic = (diacritics)j;
	  break;
	}
      }
    }

    // first, get the last_consonant
    last_consonant = -1;
    len = syllable.size();
    for (i = 0;last_consonants[i] != 0; i++) {
      char *pattern = last_consonants[i];
      int pattern_len = strlen(pattern);

      if (len > pattern_len && 
	  syllable.substr(len-pattern_len) == pattern) {
	last_consonant = i;
	syllable.erase(len-pattern_len);
	break;
      }
    }

    // then get the first_consonant
    first_consonant = -1;
    len = syllable.size();
    for (i = 0;first_consonants[i] != 0; i++) {
      char *pattern = first_consonants[i];
      int pattern_len = strlen(pattern);

      if (len >= pattern_len && 	// equal is possible
	  syllable.substr(0,pattern_len) == pattern) {
	first_consonant = i;
	syllable.erase(0,pattern_len);
	break;
      }
    }

    // check if syllable is empty. 
    // if it is, we can't use this last_consonant.
    // find the next last_consonant
    if (syllable.empty()) {
      if (last_consonant != -1)
	return false;		// bad syllable

      syllable = last_consonants[last_consonant];
      len = syllable.size();
      int start = last_consonant+1;
      last_consonant = -1;
      for (i = start;last_consonants[i] != 0; i++) {
	char *pattern = last_consonants[i];
	int pattern_len = strlen(pattern);

	if (len > pattern_len && 
	    syllable.substr(len-pattern_len) == pattern) {
	  last_consonant = i;
	  syllable.erase(len-pattern_len);
	  break;
	}
      }
    }

    // get vowel
    vowel = -1;
    len = syllable.size();
    if (len == 0)
      return false;		// bad syllable
    for (i = 0;vowels[i] != 0; i++) {
      char *pattern = vowels[i];
      int pattern_len = strlen(pattern);

      if (len >= pattern_len && 	// equal is possible
	  syllable.substr(len-pattern_len) == pattern) {
	vowel = i;
	syllable.erase(len-pattern_len);
	break;
      }
    }

    // the last is vowel
    padding_vowel = -1;
    len = syllable.size();
    if (len == 0)
      return false;
    for (i = 0;padding_vowels[i] != 0; i++) {
      char *pattern = padding_vowels[i];
      int pattern_len = strlen(pattern);

      if (syllable == pattern) {
	padding_vowel = i;
	syllable = "";
	break;
      }
    }

    return vowel != -1 && syllable.empty();
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

}
