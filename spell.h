#include "dictionary.h"
#include "wfst.h"

struct Suggestion {
  int id;
  vector<Dictionary::strid> suggestions;
};
typedef vector<Suggestion> Suggestions;

namespace Spell {
  void check1(Sentence &st,Suggestions &s);
  void check2(Sentence &st,Segmentation &seg,Suggestions &s);
}

