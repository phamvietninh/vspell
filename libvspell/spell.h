#include "dictionary.h"
#include "wfst.h"

struct Suggestion {
  int id;
  std::vector<strid> suggestions;
};
typedef std::vector<Suggestion> Suggestions;

//namespace Spell {
void spell_check1(Sentence &st,Suggestions &s);
void spell_check2(Sentence &st,Segmentation &seg,Suggestions &s);
//}

