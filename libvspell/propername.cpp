#include "propername.h"		// -*- tab-width: 2 -*-
#include "dictionary.h"
#include "syllable.h"
#include <libsrilm/File.h>
#include <set>

//using namespace Dictionary;
using namespace std;

bool find_capital_words(Sentence &st,int start,int &pos,int &len);

static set<strid> propernames;

bool proper_name_init()
{
	File ifs("pname","rt");

	if (ifs.error())
		return false;

	char *line;
	while ((line = ifs.getline()) != NULL) {
		while (line[strlen(line)-1] == '\n' || line[strlen(line)-1] == '\r' || line[strlen(line)-1] == ' ')
			line[strlen(line)-1] = 0;
		propernames.insert(get_sarch()[get_std_syllable(line)]);
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
  st[npos].cid = get_sarch()["<propername>"];//proper_name_id;
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
					e.node = get_special_node(PROPER_NAME_ID);
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
		e.node = get_special_node(PROPER_NAME_ID);
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

bool is_first_capitalized_word(const char *input)
{
	uint i,n = strlen(input);
	// check for upper-case chars
	for (i = n-1;i >= 0;i --)
		if (viet_toupper(input[i]) == input[i])
			break;
	return i == 0;
}

bool is_all_capitalized_word(const char *input)
{
	uint i,n = strlen(input);
	// check for upper-case chars
	for (i = n-1;i >= 0;i --)
		if (viet_toupper(input[i]) != input[i])
			return false;
	return true;
}

bool is_lower_cased_word(const char *input)
{
	uint i,n = strlen(input);
	// check for upper-case chars
	for (i = 0;i < n;i ++)
		if (viet_toupper(input[i]) == input[i])
			return false;
	return true;
}

bool is_first_capitalized_word(const strid_string &s)
{
	if (s.empty())
		return true;

	uint i,n = s.size();

	if (is_first_capitalized_word(sarch[s[0]])) {
		for (i = 1;i < n;i ++)
			if (!is_lower_cased_word(sarch[s[i]]))
				return false;
		return true;
	}

	return false;
}

bool is_all_capitalized_word(const strid_string &s)
{
	if (s.empty())
		return true;

	uint i,n = s.size();

	if (is_all_capitalized_word(sarch[s[0]])) {
		for (i = 1;i < n;i ++)
			if (!is_all_capitalized_word(sarch[s[i]]))
				return false;
		return true;
	}

	return false;
}

bool is_lower_cased_word(const strid_string &s)
{
	if (s.empty())
		return true;

	uint i,n = s.size();

	if (is_lower_cased_word(sarch[s[0]])) {
		for (i = 1;i < n;i ++)
			if (!is_lower_cased_word(sarch[s[i]]))
				return false;
		return true;
	}

	return false;
}

