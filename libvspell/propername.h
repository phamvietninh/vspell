#ifndef __PROPER_NAME_H__
#define __PROPER_NAME_H__

#ifndef __SPELL_H__
#include "spell.h"
#endif

#include <set>

bool proper_name_init();
void mark_proper_name(const Sentence &sent,std::set<WordEntry> &we);
//void mark_proper_name(Sentence &st);

#endif
