#ifndef __PROPER_NAME_H__
#define __PROPER_NAME_H__

#ifndef __SPELL_H__
#include "spell.h"
#endif
#ifndef __MYSTRING_H__
#include "mystring.h"
#endif

#include <set>

bool proper_name_init();
void mark_proper_name(const Sentence &sent,std::set<WordEntry> &we);
bool is_first_capitalized_word(const char *s);
bool is_all_capitalized_word(const char *s);
bool is_lower_cased_word(const char *s);
bool is_first_capitalized_word(const strid_string &s);
bool is_all_capitalized_word(const strid_string &s);
bool is_lower_cased_word(const strid_string &s);
inline bool is_valid_word_form(const strid_string &s) {
  return
    is_first_capitalized_word(s) ||
    is_all_capitalized_word(s) ||
    is_lower_cased_word(s);
}
inline bool is_valid_word_form(const char *s) {
  return
    is_first_capitalized_word(s) ||
    is_all_capitalized_word(s) ||
    is_lower_cased_word(s);
}
//void mark_proper_name(Sentence &st);

#endif
