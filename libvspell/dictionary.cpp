#include "config.h"		// -*- tab-width: 2 -*-
#include <iostream>
#include <stdlib.h>
#include <libsrilm/File.h>
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
#include <libsrilm/SArray.cc>
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
static LeafNode* special_leaves[TOTAL_ID];

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
/*
	WordNode::~WordNode()
	{
	if (info)
	delete info;
	delete nodes;
	}
*/


/*
bool WordNode::save(const char* filename)
{
	ofstream ofs(filename);

	if (!ofs.is_open())
		return false;

	typedef pair<WordNodePtr,vector<strid> > trace_type;
	vector<trace_type> traces;
	traces.push_back(make_pair(this,vector<strid>()));

	while (!traces.empty()) {
		trace_type trace = traces.back();
		traces.pop_back();
		WordNodePtr trace_ptr = trace.first;
		node_map_iterator iter(*trace_ptr->nodes);
		strid id;
		WordNodePtr *pnode;
		while ((pnode = iter.next(id))) {
			trace_type newtrace = make_pair(*pnode,trace.second);
			newtrace.second.push_back(id);
			traces.push_back(newtrace);
		}

		if (trace_ptr->get_prob() >= 0) {
			int i,n = trace.second.size();
			for (i = 0;i < n;i ++) {
				if (i) ofs << " ";
				ofs << sarch[trace.second[i]];
			}
			ofs << "#" << "N" << "#" << trace_ptr->get_prob() << " " << trace_ptr->get_a() << "/" << trace_ptr->get_b() << endl;
		}
	}
	return true;
}

WordNodePtr WordNode::create_next(strid str)
{
	WordNodePtr ptr;
	//  nodes[str].reset(new WordNode);
	*syllable_dict.insert(str) = 1;
	ptr = *nodes->insert(str) = new WordNode(str);
	//  ptr->parent = this;
	return ptr;
}

WordNodePtr FuzzyWordNode::create_next(strid str)
{
	WordNodePtr ptr;
	//  nodes[str].reset(new WordNode);
	*syllable_dict.insert(str) = 1;
	ptr = *nodes->insert(str) = new FuzzyWordNode(str);
	//  ptr->parent = this;
	return ptr;
}

WordNodePtr WordNode::get_next(strid str) const
{
	WordNodePtr *pnode = nodes->find(str);
	return pnode == NULL ? NULL : *pnode;
}

void WordNode::set_prob(float _prob)
{
	// ASSERT: info != NULL
	info->prob = _prob;
}
	

int WordNode::get_syllable_count() const
{
	ASSERT(info.get());
	int n;
	for (n = 0;info->syllables[n] != Vocab_None;n ++);
	return n;
}

void WordNode::get_syllables(vector<strid> &syllables) const
{
	ASSERT(info.get());
	int n;
	for (n = 0;info->syllables[n] != Vocab_None;n ++)
		syllables.push_back(info->syllables[n]);
}

WordNodePtr WordNode::follow_syllables(const vector<strid> &syllables)
{
	int n = syllables.size();
	int i;
	WordNodePtr root = this;
	for (i = 0;i < n && root != NULL;i ++)
		root = root->get_next(syllables[i]);
	return root;
}

bool FuzzyWordNode::fuzzy_get_next_with_ed(strid str,
																					 vector<DistanceNode>& _nodes,
																					 const char *str_data,
																					 bool parsable) const
{
	node_map_iterator iter(*nodes);
	strid id;
	WordNodePtr *pnode; 
	int ed_value;
	int str_len = strlen(str_data);
	bool ret = false;

	while ((pnode = iter.next(id))) {
		const char *iter_str = sarch[id];
		int iter_size = strlen(iter_str);
		int thresold;
			
		thresold = (!parsable) ? ED_THRESOLD2 : ED_THRESOLD1;
			
		if (thresold > 0 &&
				(iter_size - str_len >= thresold ||
				 str_len - iter_size >= thresold))
			continue;
			
		ed_value = ed(iter_str,iter_size,str_data,str_len);
			
		if (ed_value <= thresold) {	// if (id == str)
			_nodes.push_back(*pnode);
			_nodes.back().distance = ed_value;
			ret = true;
		}
	}
	return ret;
}


bool FuzzyWordNode::fuzzy_get_next_with_syllable(strid str,
																								 vector<DistanceNode>& _nodes,
																								 const Syllable &_syll) const
{
	vector<confusion_set>& confusion_sets = get_confusion_sets();
	int i,j,m,n = confusion_sets.size();
	bool ret = false;
	set<Syllable> syllset,syllset2;

	syllset2.insert(_syll);
	while (!syllset2.empty()) {
		const Syllable sy = *syllset2.begin();
		syllset2.erase(syllset2.begin());
		
		if (syllset.find(sy) != syllset.end())
			continue;								// we already matched&applied this syllable

		//cerr << sy << endl;
		syllset.insert(sy);
		
		
		vector<Syllable> sylls;
		// match & apply
		for (i = 0;i < n;i ++) {
			m = confusion_sets[i].size();
			for (j = 0;j < m;j ++)
				if (confusion_sets[i][j].match(sy))
					break;
			if (j < m) {
				for (j = 0;j < m;j ++)
					confusion_sets[i][j].apply(sy,sylls);
			}
		}
		//sort(sylls.begin(),sylls.end());
		//sylls.erase(unique(sylls.begin(),sylls.end()),sylls.end());
		//set_difference(sylls.begin(),sylls.end(),
		//							 syllset.begin(),syllset.end(),
		//							 insert_iterator<set<Syllable> >(syllset2,syllset2.begin()));
		copy(sylls.begin(),sylls.end(), inserter(syllset2,syllset2.begin()));
		//cerr << "sylls ";copy(sylls.begin(),sylls.end(),ostream_iterator<Syllable>(cerr)); cerr << endl;
		//cerr << "syllset ";copy(syllset.begin(),syllset.end(),ostream_iterator<Syllable>(cerr)); cerr << endl;
		//cerr << "syllset2 ";copy(syllset2.begin(),syllset2.end(),ostream_iterator<Syllable>(cerr)); cerr << endl;
	}
		
	// move to _nodes
	//copy(syllset.begin(),syllset.end(),ostream_iterator<Syllable>(cerr)); cerr << endl;
	set<Syllable>::iterator iter;
	for (iter = syllset.begin();iter != syllset.end(); ++ iter) {
		WordNodePtr* pnode = nodes->find(iter->to_std_id());
		if (pnode) {
			_nodes.push_back(*pnode);
			_nodes.back().distance = 1;
			ret = true;
		}
	}
		
	return ret;
}


// the get_next()'s result must be included.
bool FuzzyWordNode::fuzzy_get_next(strid str,
																	 vector<DistanceNode>& _nodes) const
{
	if (!fuzzy)
		return WordNode::fuzzy_get_next(str,_nodes);

	const char *str_data = sarch[str];

	Syllable syll;
	bool parsable = syll.parse(str_data);
	bool is_syllable = is_syllable_exist(str);

	if (!parsable || !is_syllable) {
		// fuzzy_get_next_with_ed(str,_nodes,str_data,parsable);
		// _nodes.push_back(*root->nodes->find(unk_id));
		// return true;							// what is this? ;)
		return false;
	} else
		return fuzzy_get_next_with_syllable(str,_nodes,syll);
}

bool WordNode::fuzzy_get_next(strid str,
															vector<DistanceNode>& _nodes) const
{
	WordNodePtr* pnode = nodes->find(str);
	if (pnode) {
		_nodes.push_back(*pnode);
		return true;
	}
	return false;
}

void WordNode::recalculate()
{
	vector<WordNodePtr> traces;
	traces.push_back(this);

	while (!traces.empty()) {
		WordNodePtr trace = traces.back();
		traces.pop_back();
		WordNodePtr trace_ptr = trace;
		node_map_iterator iter(*trace_ptr->nodes);
		strid id;
		WordNodePtr* pnode;
		while ((pnode = iter.next(id))) {
			WordNodePtr newtrace = *pnode;
			traces.push_back(newtrace);
		}

		if (trace_ptr->get_prob() >= 0) {
			if (trace_ptr->info->a > 0 && trace_ptr->info->b > 0)
				trace_ptr->info->prob = -log10((double)trace_ptr->info->a/(double)trace_ptr->info->b);
			else
				trace_ptr->info->prob = -log10(1.0/10.0);
			if (trace_ptr->info->prob == -0) trace_ptr->info->prob = 0;
			trace_ptr->info->a = trace_ptr->info->b = 0;
		}
	}
}
*/
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
/*
void WordNode::dump_next(std::ostream &os) const
{
	node_map_iterator iter(*nodes);
	strid id;
	WordNodePtr *pnode;
	while ((pnode = iter.next(id))) {
		os << (*pnode)->id << " ";
	}
}
std::ostream& operator << (std::ostream &os,const WordNode::DistanceNode &dnode)
{
	os << *dnode.node;
	return os;
}
std::ostream& operator << (std::ostream &os,const WordNode &node)
{
	std::vector<strid> syll;
  node.get_syllables(syll);
	for (std::vector<strid>::size_type i = 0;i < syll.size();i ++) {
		if (i)
			os << " ";
		os << sarch[syll[i]] << "(" << syll[i] << ") ";
	}
	return os;
}

*/
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

LeafNode* get_special_node(int id)
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
