#ifndef __SYLLABLE_H__ // -*- tab-width: 2 mode: c++ -*-
#define __SYLLABLE_H__

#ifndef __DICTIONARY_H__
#include "dictionary.h"
#endif

#ifndef __VECTOR__
#include <vector>
#endif

class Syllable {
public:
  enum diacritics {
    None = 0,
    Acute,
    Grave,
    Hook,
    Tilde,
    Dot  
  };
  enum {
    First_Consonant,
    Padding_Vowel,
    Vowel,
    Last_Consonant,
    Diacritic
  };

  int components[5];
  //    int 	first_consonant;
  //    int 	padding_vowel;
  //    int 	vowel;
  //    int 	last_consonant;
  //    diacritics 	diacritic;

  Syllable(const char*  _first_consonant = NULL,
	   const char*  _padding_vowel = NULL,
	   const char*  _vowel = NULL,
	   const char*  _last_consonant = NULL,
	   int diacritic = -1);


  bool match(const Syllable &sample);
  void apply(const Syllable &sample,std::vector<Syllable> &output);
  bool parse(const char *str);
  void standardize(std::string str);
  void print();
  strid to_id();
  std::string to_str();
};

extern char *vowels[];
extern char *first_consonants[];
extern char *last_consonants[];
extern char *padding_vowels[];

#endif
