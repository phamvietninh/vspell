#include "config.h"		// -*- tab-width: 2 -*-
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include "wfst.h"
#include "spell.h"

using namespace std;
using namespace Dictionary;

/*

	The process:
	1. Separate "words" by spaces.
	2. Sentence segmentation. 1 Sentence -> n Sentence.
	3. Punctuation seperation.
	4. Foreign/Abbreviation detection.
	5. Proper name detection.
	6. Generalization (into class e.g. number_class, foreign_class ...). Try to
	   generalize all capitalized words.
	6* Syllable checking.
	7. Find all possible (misspelled) words. (**)
	8. "pre-separate" sentence into phrases.
	9. Word segmentation. (**)
	10. Find the best segmentation.
	10* Word checking.

 */

namespace Spell {

  void check1(Sentence &st,Suggestions &sugg)
  {
    int i,n = st.get_syllable_count();
    for (i = 0;i < n;i ++) {
      strid id = st[i].get_cid();
      if (sarch.in_dict(id))
				continue;

      st[i].sid = unk_id;

      VocabString s = sarch[id];
      if (strlen(s) == 1 && viet_isupper(s[0]) && viet_islower(s[0])) {
				st[i].sid = sarch["<PUNCT>"];
				continue;
      }

      Suggestion _s;
      _s.id = i;
      sugg.push_back(_s);
    }
  }

  void check2(Sentence &st,Segmentation &seg,Suggestions &sugg)
  {
    int i,n = seg.items.size();
    int cc = 0;

    for (i = 0;i < n;i ++) {
      vector<strid> sylls;
      int len = seg.items[i].state->get_syllable_count();
      if (len == 1) {
	cc += len;
	continue;
      } 

      int start;
      WordNodePtr node(get_root());
      for (start = 0;start < len && node != NULL; start ++)
	node = node->get_next(st[start+cc].cid);

      cc += len;
      if (node == NULL) {
	Suggestion _s;
	_s.id = i;
	sugg.push_back(_s);
      }
    }
  }


}
