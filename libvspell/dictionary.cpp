#include "config.h"		// -*- tab-width: 2 -*-
#include <iostream>
#include <libsrilm/File.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include "dictionary.h"
#include "wordnode.h"
#include "distance.h"
#include <stdlib.h>
#include <math.h>
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
syllable_dict_type syllable_dict;
WordNodePtr root;
strid unk_id;

StringArchive sarch;
Ngram ngram(sarch.get_dict(),2);
NgramStats stats(sarch.get_dict(),2);

bool syllable_init();

bool dic_init(WordNodePtr _root)
{
	syllable_init();
	ed_init();
	root = _root;
	unk_id = sarch["<UNK>"];	// should be in Dictionary::load
	// if 
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
	delete root;
}

WordNodePtr get_root()
{
	return root;
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
bool WordNode::load(const char* filename)
{
	File ifs(filename,"rt");

	if (ifs.error())
		return false;

	int nr_lines = 0;
	char *line;
	int pos,start,len,tmp_char;
	char *str_pos;
	vector<string> toks;
	while ((line = ifs.getline())) {
		start = 0;
		len = strlen(line);
		toks.clear();
		while (start < len) {
			str_pos = strchr(line+start,'#');
			if (str_pos == NULL)
				pos = len;
			else
				pos = str_pos - line;
			tmp_char = line[pos];
			line[pos] = 0;
			toks.push_back(line+start);
			line[pos] = tmp_char;
			start = pos+1;
		}

		if (!toks.size())
			continue;			// unrecoverable error
		if (toks.size() < 2)
			toks.push_back("N");	// assume N
		if (toks.size() < 3)
			toks.push_back("0");	// assume 0

		start = 0;
		len = toks[0].size();
		WordNodePtr node(this),cnode(this);
		vector<VocabIndex> syllables;
		vector<VocabIndex> csyllables;
		while (start < len) {
			pos = toks[0].find(' ',start);
			if (pos == string::npos)
				pos = len;
			else
				toks[0][pos] = '_';	// prepared for getting word id
			string s = toks[0].substr(start,pos-start);
			VocabIndex id = sarch[s];
			syllables.push_back(id);
			WordNodePtr next = node->get_next(id);
			if (!next) {		// create new
				next = node->create_next(id);
			}
			node = next;

			transform(s.begin(),s.end(),s.begin(),viet_tolower);
			VocabIndex cid = sarch[s];
			csyllables.push_back(cid);
			WordNodePtr cnext = cnode->get_next(cid);
			if (!cnext) {		// create new
				cnext = cnode->create_next(cid);
			}
			cnode = cnext;
			start = pos+1;
		}
		if (toks[2].find(' ') != string::npos)
			toks[2].erase(toks[2].find(' '));

		node->info = new WordInfo;
		node->info->id = sarch[toks[0]];
		int i,nr_syllables = syllables.size();
		node->info->syllables = new VocabIndex[nr_syllables+1];
		node->info->syllables[nr_syllables] = Vocab_None;
		for (int i = 0;i < nr_syllables;i ++)
			node->info->syllables[i] = syllables[i];
		node->set_prob(atof(toks[2].c_str()));

		if (csyllables != syllables) {
			transform(toks[0].begin(),toks[0].end(),toks[0].begin(),viet_tolower);
			cnode->info = new WordInfo;
			cnode->info->id = sarch[toks[0]];
			nr_syllables = csyllables.size();
			cnode->info->syllables = new VocabIndex[nr_syllables+1];
			cnode->info->syllables[nr_syllables] = Vocab_None;
			for (i = 0;i < nr_syllables;i ++)
				cnode->info->syllables[i] = csyllables[i];
			cnode->set_prob(atof(toks[2].c_str()));
		}
		nr_lines ++;
	}

	return true;
}

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
	

int WordNode::get_syllable_count()
{
	ASSERT(info != NULL);
	int n;
	for (n = 0;info->syllables[n] != Vocab_None;n ++);
	return n;
}

void WordNode::get_syllables(vector<strid> &syllables)
{
	// ASSERT: info != NULL
	int n;
	for (n = 0;info->syllables[n] != 0;n ++)
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

typedef vector<Syllable> confusion_set;
extern vector<confusion_set> confusion_sets;

bool FuzzyWordNode::fuzzy_get_next_with_syllable(strid str,
																								 vector<DistanceNode>& _nodes,
																								 const Syllable &syll) const
{
	int i,j,m,n = confusion_sets.size();
	bool ret = false;
	vector<Syllable> sylls;
		
	// match & apply
	for (i = 0;i < n;i ++) {
		m = confusion_sets[i].size();
		for (j = 0;j < m;j ++)
			if (confusion_sets[i][j].match(syll))
				break;
		if (j < m) {
			for (j = 0;j < m;j ++)
				confusion_sets[i][j].apply(syll,sylls);
		}
	}
		
	// move to _nodes
	n = sylls.size();
	for (i = 0;i < n;i ++) {
		WordNodePtr* pnode = nodes->find(sylls[i].to_id());
		if (pnode) {
			_nodes.push_back(*pnode);
			_nodes.back().distance = 1;
			ret = true;
		}
	}
		
	return ret;
}


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

strid StringArchive::operator[] (VocabString s)
{
	VocabIndex vi = dict.getIndex(s);
	if (vi != Vocab_None)
		return vi;
	if (blocked) {
		vi = rest->getIndex(s);
		if (vi == Vocab_None) 
			return rest->addWord(s)+dict.numWords();
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
	}
*/
