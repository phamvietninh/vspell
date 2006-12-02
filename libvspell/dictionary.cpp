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

#define ED_THRESOLD1 1
#define ED_THRESOLD2 2

using namespace std;

/*
	namespace Dictionary {
*/

static strid special_ids[TOTAL_ID];
static LeafNNode* special_leaves[TOTAL_ID];

static LM ngram, syngram;
static map<strid,strid_string> pnames;
WordArchive warch;

bool syllable_init();
void viet_init();

bool dic_init()
{
	viet_init();
	syllable_init();
	ed_init();
	int i;
	char *specials[TOTAL_ID] = {"<opaque>","<punct>","<prop>","<s>","</s>","<poem>","<digit>","<leaf>"};
	for (i = 0;i < TOTAL_ID;i ++) {
		special_ids[i] = sarch[specials[i]];
		special_leaves[i] = warch.add_special_entry(special_ids[i]);
	}
	proper_name_init();
	warch.init();
	return true;
}

void LM::dump()
{
	// FIXME
}

LM::LM()
{
	lm = NewModel (0, 0, 0);
	lm->word_str = NULL;
	lm->n_word_str = 0;

	oov_ht.size = 0;
	oov_ht.inuse = 0;
	oov_ht.tab = NULL;
}

LM::~LM()
{
	if (lm != NULL) {
		lm3g_free(lm);
		lm = NULL;
	}
}

bool LM::read(const char *filename)
{
	if (lm != NULL)
		lm3g_free(lm);
	lm = lm3g_read(filename);
	oov.clear();
	blocked = 0;
	return lm != NULL;
}

#define MAXPROB -9999.0

double LM::wordProb(VocabIndex w1, VocabIndex *wn)
{
	if (w1 < 1 || w1 >= lm->n_word_str+1) return MAXPROB;
	if (wn == NULL || wn[0] == 0) return lm3g_ug_score(lm, w1-1);
	if (wn[0] < 1 || wn[0] >= lm->n_word_str+1) return MAXPROB;
	if (wn[1] == 0) return lm3g_bg_score(lm, wn[0]-1, w1-1);
	if (wn[1] < 1 || wn[1] >= lm->n_word_str+1) return MAXPROB;
	if (wn[2] == 0) return lm3g_tg_score(lm, wn[0]-1, wn[1]-1, w1-1);
	return MAXPROB;
}

void dic_clean()
{
}

bool is_word_exist(const std::string &word)
{
	return false;
}

float get_word(const std::string &word)
{
	return 0;
}


strid LM::operator[] (const char* s)
{
	VocabIndex vi = lm3g_wstr2wid(lm, s);
	if (vi > 0)
		return vi;
	caddr_t val;
	if (blocked > 0 && !hash_lookup(&oov_ht, s, &val))
		return (strid)val;
	s = strdup(s);
	oov.push_back(s);
	vi = 1+lm->n_word_str + oov.size()-1; // the first one is a reserved id
	hash_add (blocked > 0 ? &oov_ht : &(lm->HT), s, (caddr_t) vi);
	return vi;
}

const char* LM::operator[] (strid i)
{
	if (i >= 1 && i < 1+lm->n_word_str)
		return lm->word_str[i-1];
	if (i >= 1+lm->n_word_str && i < 1+lm->n_word_str+oov.size())
		return oov[i-(1+lm->n_word_str)];
	return NULL;
}

void LM::set_blocked(bool _blocked)
{
	blocked = _blocked ? oov.size() : 0;
	clear_oov();
}

void LM::clear_oov()
{
	if (blocked > 0) {
		int i,len = oov.size();
		hash_free(&oov_ht);
		oov_ht.size = 0;
		oov_ht.inuse = 0;
		oov_ht.tab = NULL;
		for (i = blocked;i < len;i ++)
			free((void*)oov[i]);
		oov.resize(blocked);
	}
}

bool LM::in_dict(const char* s)
{
	VocabIndex vi = lm3g_wstr2wid(lm, s);
	return vi > 0 && vi < 1+lm->n_word_str;
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

LM& get_ngram()
{
	return ngram;
}

LM& get_syngram()
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

double LogPtoProb(double v)
{
	return exp10(v);
}

/*
	}
*/
