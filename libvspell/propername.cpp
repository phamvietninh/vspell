#include "propername.h"		// -*- tab-width: 2 -*-
#include "dictionary.h"
#include <libsrilm/File.h>
#include <set>

//using namespace Dictionary;
using namespace std;

bool find_capital_words(Sentence &st,int start,int &pos,int &len);

set<strid> propernames;

bool proper_name_init()
{
	File ifs("propername.lst","rt");

	if (ifs.error())
		return false;

	char *line;
	while ((line = ifs.getline()) != NULL) {
		while (line[strlen(line)-1] == '\n' || line[strlen(line)-1] == '\r' || line[strlen(line)-1] == ' ')
			line[strlen(line)-1] = 0;
		propernames.insert(sarch[get_dic_syllable(line)]);
	}
	return true;
}

/*
  void mark_proper_name(Sentence &st)
  {
  // find all possibilities:
  // a proper name is a name consisting of words,
  // which are started with a capitalized character. 
  // e.g. Cong hoa Xa hoi Chu nghia Viet Nam
  // hmm.. these should be treated like other words for spelling checking.

  // individual name is easier: Nguyen Thai Ngoc Duy
  // these are usually started with common last name like Nguyen, Tran, ...
  // there are 2-5 words in a name.

  // there are 2 approaches:
  // one tries to find contiguous capitalized word then check for proper name
  // other tries to find last name first and find the rest.
  // which one is better?

  // if one started with a capital word, then we should check the rest with
  // our proper name list.

  // here i choose the first approach so that i can check for other proper
  // names in addition to personal names.

  int pos = 0,len = 0,npos,nlen;
  while (1) {
  if (!find_capital_words(st,pos+len,npos,nlen))
  break;

  // check for last name
  if (last_names.find(st[npos].id) != last_names.end()) {
  // mark as a personal name.
  st[npos].span = nlen;	// skip next nlen syllables.
  st[npos].cid = sarch["<propername>"];//proper_name_id;
  }

  // check for other proper names
  pos = npos;
  len = nlen;
  }
  }
*/

void mark_proper_name(const Sentence &sent,set<WordEntry> &we)
{
	int i,n;
	int start = -1;
	n = sent.get_syllable_count();

	for (i = 0;i < n;i ++) {
		if (propernames.find(sent[i].get_cid()) != propernames.end()) {
			if (start == -1)
				start = i;
		} else {
			if (start != -1) {
				if (i-start > 1) {
					WordEntry e;
					e.pos = start;
					e.len = i-start;
					e.fuzid = 0;
					e.node = get_root()->get_next(proper_name_id);
					we.insert(e);
				}
				start = -1;
			}
		}
	}

	// i == n at the moment
	if (start != -1) {
		WordEntry e;
		e.pos = start;
		e.len = i-start;
		e.fuzid = 0;
		e.node = get_root()->get_next(proper_name_id);
		we.insert(e);
		start = -1;
	}
}

bool find_capital_words(Sentence &st,int start,int &pos,int &len)
{
	int i, sz = st.get_syllable_count();
	bool found = false;
	string sent = st.get();
	for (i = start;i < sz;i ++) {
		if (viet_isupper(sent[st[i].start])) {
			if (!found) {
				found = true;
				pos = i;
				len = 1;
			} else {
				len ++;
			}
		} else {
			if (found)
				return true;
		}
	}
	return found;
}
