// -*- coding: vscii mode: c++ -*-
#include <string>
#include <iostream>
#include <string.h>

using namespace std;

char *vowels[] = {
  "iª","yª","ia",
  "_¬","_a","u«","ua",
  "a","¨","©",
  "e","ª",
  "o","«","¬",
  "u","_","i","y",
  NULL
};

char *first_consonants[] = {	// longest first
  "ngh",
  "nh","ng","ch","gh","ph","th","tr","gi","kh",
  "c","m","n"/*,"p"*/,"t","b","k","q","d","®","g","h","l","r","s","v","x",
  NULL
};

char *last_consonants[] = {	// longest first
  "nh","ng","ch",/*"gh","ph","th","tr","gi","kh",*/
  "c","m","n","p","t",/*"b","k","q","d","®","g","h","l","r","s","v","x",*/
  // these are semivowels, not consonants.
  "i","y","o","u",
  NULL
};

char *padding_vowels[] = {
  "o","u",
  NULL
};

enum diacritics {
  none = 0,
  acute,
  grave,
  hook,
  tilde,
  dot  
};

char *diacritic_table[6] = {
  "a©¨eªio«¬u_y",
  "¸Ê¾ĞÕİãèíóøı",
  "µÇ»ÌÒ×ßåêïõú",
  "¶È¼ÎÓØáæëñöû",
  "·É½ÏÔÜâçìò÷ü",
  "¹ËÆÑÖŞäéîôùş",
};

char *case_table[2] = {
  "¸µ¶·¹©ÊÇÈÉË¨¾»¼½ÆĞÌÎÏÑªÕÒÓÔÖİ×ØÜŞãßáâä«èåæçé¬íêëìîóïñòô_øõö÷ùıúûüş®", 
  "ƒ€‚„¢$ÄÂÃ†¡À¯º¿…Š‡ˆ‰‹£ÚÅÍÙŒ‘•’“”–¤ÿÛàğ—¥›˜™šœŸ¦q§", 
};

class Syllable {
public:
  int 		first_consonant;
  int 		padding_vowel;
  int 		vowel;
  int 		last_consonant;
  diacritics 	diacritic;

  bool parse(const char *str);
  void standardize(std::string str);
};

int viet_toupper(int ch);
int viet_tolower(int ch);
bool viet_isupper(int ch);
bool viet_islower(int ch);

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
  // We just need to eliminate "ßa", "ße", "ïa"

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
int main()
{
  Syllable i;

  cout << i.parse("ngoĞo") << ": ";
  cout << first_consonants[i.first_consonant] << " ";
  cout << padding_vowels[i.padding_vowel] << " ";
  cout << vowels[i.vowel] << " "; 
  cout << last_consonants[i.last_consonant] << endl;
  return 0;
}


int viet_toupper(int ch)	// must be sure ch is a character
{
  return  ch >= 'a' && ch <= 'z' ?
    ch-32 :
    case_table[1][strchr(case_table[0],ch)-case_table[0]];
}
int viet_tolower(int ch)	// must be sure ch is a character
{
  return ch >= 'A' && ch <= 'Z' ?
    ch+32 :
    case_table[0][strchr(case_table[1],ch)-case_table[1]];
}
bool viet_isupper(int ch)
{
  return (ch >= 'A' && ch <= 'Z') ||
    strchr(case_table[1],ch) != NULL;
}

bool viet_islower(int ch)
{
  return (ch >= 'a' && ch <= 'z') ||
    strchr(case_table[0],ch) != NULL;
}
