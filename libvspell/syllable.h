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
	std::string scomponents[5];
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
	Syllable(const Syllable &sy);


  bool match(const Syllable &sample);
  void apply(const Syllable &sample,std::vector<Syllable> &output);
  bool parse(const char *str);
  void standardize(std::string str);
  strid to_id() const;
  strid to_std_id() const;
  std::string to_str() const;
  std::string to_std_str() const;
	friend std::ostream& operator << (std::ostream &os,const Syllable &sy);
	friend bool operator < (const Syllable &s1,const Syllable &s2);
	friend bool operator == (const Syllable &s1,const Syllable &s2);
};

//extern char *vowels[];
//extern char *first_consonants[];
//extern char *last_consonants[];
//extern char *padding_vowels[];
std::string get_std_syllable(const std::string &str);
std::string get_dic_syllable(const std::string &str);
std::string get_lowercased_syllable(const std::string &str);
typedef std::vector<Syllable> confusion_set;
std::vector<confusion_set>& get_confusion_sets();

#endif
