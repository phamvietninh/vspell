#include "config.h"		// -*- tab-width: 2 -*-
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include "dictionary.h"
#include "wordnode.h"
#include "distance.h"
#include "propername.h"
#include <math.h>
#include <iterator>
#ifndef _SArray_cc_
#include <SArray.cc>
#endif

#define ED_THRESOLD1 1
#define ED_THRESOLD2 2

using namespace std;

/*
	namespace Dictionary {
*/

typedef SArray<strid,float> syllable_dict_type;
typedef SArrayIter<strid,float> syllable_dict_iterator;
static syllable_dict_type syllable_dict;
static strid special_ids[TOTAL_ID];
static LeafNNode* special_leaves[TOTAL_ID];

#ifdef sarch
#undef sarch
#endif

static StringArchive sarch;
static Ngram ngram(sarch.get_dict(),3);
static Ngram syngram(sarch.get_dict(),2);
static map<strid,strid_string> pnames;
WordArchive warch;

bool syllable_init();
void viet_init();

bool dic_init()
{
	viet_init();
	syllable_init();
	ed_init();
	sarch["<reserved>"]; // 0, don't use
	int i;
	char *specials[TOTAL_ID] = {"<opaque>","<punct>","<prop>","<s>","</s>","<poem>","<digit>","<leaf>"};
	for (i = 0;i < TOTAL_ID;i ++) {
		special_ids[i] = sarch[specials[i]];
		special_leaves[i] = warch.add_special_entry(special_ids[i]);
	}
	proper_name_init();
	return true;
}

void StringArchive::dump()
{
	FILE *fp = fopen("dic.dump","wt");
	int i,n = dict.numWords();
	for (i = 0;i < n;i ++)
		fprintf(fp,"%d %s\n",i,sarch[i]);
	fclose(fp);
}

void dic_clean()
{
}

bool is_syllable_exist(strid syll)
{
	float* pprob = syllable_dict.find(syll);
	return (pprob != NULL);
}

float get_syllable(strid syll)
{
	float* pprob = syllable_dict.find(syll);
	if(pprob == NULL)
		return 0;
	return *pprob;
}

bool is_word_exist(const std::string &word)
{
	return false;
}

float get_word(const std::string &word)
{
	return 0;
}


strid StringArchive::operator[] (VocabString s)
{
	VocabIndex vi = dict.getIndex(s);
	if (vi != Vocab_None)
		return vi;
	if (blocked) {
		vi = rest->getIndex(s);
		if (vi == Vocab_None) {
			int i = rest->addWord(s)+dict.numWords();
			//cerr << "New word " << s << " as " << i << endl;
			return i;
		}
		return vi+dict.numWords();
	}
	return dict.addWord(s);
}

VocabString StringArchive::operator[] (strid i)
{
	if (i >= dict.numWords())
		return rest->getWord(i-dict.numWords());
	return dict.getWord(i);
}

void StringArchive::set_blocked(bool _blocked)
{
	blocked = _blocked;
	if (blocked && rest == NULL)
		rest = new Vocab;
	if (!blocked && rest != NULL) {
		delete rest;
		rest = NULL;
	}
}

void StringArchive::clear_rest()
{
	if (rest) {
		delete rest;
		rest = new Vocab;
	}
}

bool StringArchive::in_dict(VocabString s)
{
	VocabIndex vi = dict.getIndex(s);
	return vi != Vocab_None;
}

strpair make_strpair(strid str)
{
	const char *s = sarch[str];
	string st(s);
	int i,len = st.size();
	for (i = 0;i < len;i ++) {
		st[i] = viet_tolower(st[i]);
	}
	strpair pair;
	pair.id = str;
	pair.cid = sarch[st];
	return pair;
}
StringArchive& get_sarch()
{
	return sarch;
}

Ngram& get_ngram()
{
	return ngram;
}

Ngram& get_syngram()
{
	return syngram;
}

strid get_id(int id)
{
	if (id < TOTAL_ID)
		return special_ids[id];
	else
		return special_ids[UNK_ID];
}

LeafNNode* get_special_node(int id)
{
	if (id < TOTAL_ID)
		return special_leaves[id];
	else
		return special_leaves[UNK_ID];
}

const std::map<strid,strid_string>& get_pnames()
{
	return pnames;
}

/*
	}
*/
