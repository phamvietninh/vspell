#include "config.h"		// -*- tab-width: 2 -*-
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include "wfst.h"
#include "spell.h"

using namespace std;

/*

The process:
1. Sentence segmentation. (sentences_split)
2. Separate "words" by spaces. (tokenize)
3. Punctuation separation. (tokenize/tokenize_punctuation)
4. Foreign/Abbreviation detection.
5. Proper name detection.
6. Generalization (into class e.g. number_class, foreign_class ...). Try to
generalize all capitalized words.
6* Syllable checking. (check1)
7. Find all possible (misspelled) words. (**) (get_all_words)
8. "pre-separate" sentence into phrases.
9. Word segmentation. (**)
10. Find the best segmentation. (segment_best)
10* Word checking. (check2)

*/
/*
	namespace Spell {
*/
void spell_check1(Sentence &st,Suggestions &sugg)
{
	int i,n = st.get_syllable_count();
	for (i = 0;i < n;i ++) {
		strid id = st[i].get_cid();
		if (sarch.in_dict(id))
			continue;

		st[i].sid = unk_id;

		VocabString s = sarch[id];
		if (strlen(s) == 1 && !viet_isalpha(s[0])) {
			st[i].sid = sarch["<PUNCT>"];
			continue;
		}

		Suggestion _s;
		_s.id = i;
		sugg.push_back(_s);
	}
}

void spell_check2(Sentence &st,Segmentation &seg,Suggestions &sugg)
{
	int i,n = seg.size();
	int cc = 0;

	for (i = 0;i < n;i ++) {
		vector<strid> sylls;
		int len = seg[i].node->get_syllable_count();
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

/*
	}
*/
