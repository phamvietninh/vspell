#include "config.h"							// -*- tab-width: 2 -*-
#include "spell.h"
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/format.hpp>
#include <set>
#include "syllable.h"
#include "propername.h"
#ifndef _SArray_cc_
#include <libsrilm/SArray.cc>
#endif


using namespace std;

WordState::WordState(const WordState &ws):dnode(ws.dnode),sent(ws.sent),fuzid(ws.fuzid),pos(ws.pos)
{
}

void WordState::get_first(WordStates &states,uint _pos)
{
	dnode.node = warch.get_root();
	pos = _pos;
	fuzid = 0;
	get_next(states,pos);
}

void ExactWordState::get_next(WordStates &states,uint i)
{
	BranchNode *branch = dnode.node->get_branch(sent[i].get_cid());
	if (branch == NULL)
		return;
	cerr << "Exact: " << get_sarch()[sent[i].get_cid()] << endl;
	states.push_back(this);
	// change the info
	dnode = branch;
}

void LowerWordState::get_next(WordStates &states,uint i)
{
	string s1,s2;
	s1 = get_sarch()[sent[i].get_cid()];
	s2 = get_lowercased_syllable(s1);
	BranchNode *branch = dnode.node->get_branch(get_sarch()[s2]);
	if (branch == NULL)
		return;
	cerr << "Lower: " << get_lowercased_syllable(get_sarch()[sent[i].get_cid()]) << endl;
	states.push_back(this);
	// change the info
	dnode = branch;
	if (s1 != s2)
		fuzid |= 1 << (i-pos);
}

void FuzzyWordState::get_next(WordStates &states,uint _i)
{
	vector<confusion_set>& confusion_sets = get_confusion_sets();
	int i,j,m,n = confusion_sets.size();
	bool ret = false;
	set<Syllable> syllset,syllset2;
	Syllable _syll;
	string s1 = get_sarch()[sent[_i].get_cid()];

	_syll.parse(s1.c_str());

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
				if (confusion_sets[i][j].match(sy)) {
					//cerr << "Match " << i << " " << j << endl;
					break;
				}
			if (j < m) {
				for (j = 0;j < m;j ++) {
					confusion_sets[i][j].apply(sy,sylls);
					//cerr << "Apply " << i << " " << j << endl;
				}
			}
		}
		copy(sylls.begin(),sylls.end(), inserter(syllset2,syllset2.begin()));
	}
		
	// move to _nodes
	//copy(syllset.begin(),syllset.end(),ostream_iterator<Syllable>(cerr)); cerr << endl;
	set<Syllable>::iterator iter;
	for (iter = syllset.begin();iter != syllset.end(); ++ iter) {
		//cerr << iter->to_std_str() << endl;
		string str = get_lowercased_syllable(iter->to_std_str());
		BranchNode::const_np_range range = dnode.node->get_nodes().equal_range(get_sarch()[str]);
		BranchNode::node_map::const_iterator pnode;
		for (pnode = range.first;pnode != range.second;++pnode)
			if (!pnode->second->is_leaf()) {
				cerr << "Fuzzy: " << iter->to_std_str() << endl;
				WordState *s = new FuzzyWordState(*this);

				// change the info
				s->dnode.node = (BranchNode*)pnode->second.get();
				if (s1 != str)
					s->fuzid |= 1 << (_i-s->pos);
				states.push_back(s);
				//cerr << nodes[ii] << endl;
			}
	}
		
	delete this;
}

/**
	Find all possible words. The whole process is divided into two
	phases. The first one is pre_construct(), which creates base words
	and store in set<WordEntry>. The second one is post_construct(),
	which does the rest. Using pre_construct(),post_construct() directly
	give chances to modify the lattice creation.

	\param sent specify the input sentence
	\param w must be cleared before calling this function
*/
void Lattice::construct(const Sentence &sent)
{
	set<WordEntry> wes;
	WordStateFactories factories;
	ExactWordStateFactory exact;
	LowerWordStateFactory lower;
	FuzzyWordStateFactory fuzzy;
	factories.push_back(&exact);
	factories.push_back(&lower);
	factories.push_back(&fuzzy);
	pre_construct(sent,wes,factories);
	mark_proper_name(sent,wes);
	post_construct(wes);
}

/**
	The first phase of lattice creation. Create all possible words to we.
 */

void Lattice::pre_construct(const Sentence &sent,set<WordEntry> &we,const WordStateFactories &f)
{
	Lattice &w = *this;
	int i,n,ii,nn,k,nnn,iii;
	int fi,fn = f.size();

	//cerr << "construct\n";

	w.st = &sent;

	WordStates states1;
	WordStates states2;
	states1.reserve(10);
	states2.reserve(10);

	n = sent.get_syllable_count();

	for (i = 0;i < n;i ++) {

		//cerr << *this << endl;

			states2.clear();

		// new states
		for (fi = 0;fi < fn;fi ++)
			f[fi]->create_new(states2,i,sent);

		// move old states to new states
		nn = states1.size();
		for (ii = 0;ii < nn;ii ++)
			// state1[ii].get_next() have to delete itself if necessary.
			states1[ii]->get_next(states2,i);

		// get completed words
		nn = states2.size();
		for (ii = 0;ii < nn;ii ++) {
			std::vector<LeafNode*> leaves;
			uint kind[2] = {MAIN_LEAF,CASE_LEAF};
			for (uint i_kind = 0;i_kind < 2;i_kind ++) {
				leaves.clear();
				states2[ii]->dnode.node->get_leaves(leaves,kind[i_kind]);
				nnn = leaves.size();
				for (iii = 0;iii < nnn;iii ++) {
					WordEntry e;
					e.pos = states2[ii]->pos;
					e.len = i-states2[ii]->pos+1;
					e.fuzid = states2[ii]->fuzid;
					e.node = leaves[iii];
					//cerr << "Add " << e << endl;
					we.insert(e);
				}
			}
		}

		states1.swap(states2);
	}

	//cerr << *this << endl;

	nn = states1.size();
	for (ii = 0;ii < nn;ii ++)
		delete states1[ii];
	//cerr << *this << endl;

}

/**
	 The second phase of lattice creation. 
 */

void Lattice::post_construct(set<WordEntry> &we)
{
	Lattice &w = *this;
	unsigned i,n,k;
	vector<bool> marks;

	n = st->get_syllable_count();
	marks.resize(n);
	set<WordEntry>::iterator iter;
	for (iter = we.begin();iter != we.end(); ++iter) {
		//cerr << get_sarch()[iter->node.node->get_id()] << " " << iter->pos << " " << iter->len << endl;
		for (k = 0; k < iter->len;k ++)
			marks[iter->pos+k] = true;
	}

	// make sure that one can go from source to destination, without gaps.
	for (i = 0;i < n;i ++)
		if (!marks[i]) { 
			WordEntry e;
			e.pos = i;
			e.len = 1;
			e.fuzid = 0;
			// if one starts with a cardinal number, then mark it number_id
			string s = get_sarch()[(*st)[i].get_cid()];
			//cerr << "Consider " << s;
			if (strchr("0123456789",s[1]) != NULL)
				e.node = get_special_node(NUMBER_ID);
			else {
				int iiii,nnnn = s.size();
				for (iiii = 0;iiii < nnnn;iiii ++)
					if (viet_ispunct(s[i]))
						break;
				if (iiii < nnnn)
					e.node = get_special_node(PUNCT_ID);
				else
					e.node = get_special_node(UNK_ID);
			}
			//cerr << " " << get_sarch()[e.node.node->get_id()] << endl;
			we.insert(e);
		}

	//copy(we.begin(),we.end(),ostream_iterator<WordEntry>(cerr));
	// copy to real _we
	n = we.size();
	w.we = boost::shared_ptr<WordEntries>(new WordEntries);
	w.we->reserve(n);
	copy(we.begin(),we.end(),back_insert_iterator<vector<WordEntry> >(*w.we));
	for (i = 0;i < n;i ++) {
		(*w.we)[i].id = i;
	}
	// build Lattice structure
	w.construct();
}

/**
	 Post-process after initialize core member of Lattice
 */

void Lattice::construct()
{
	int i_we,n_we = we->size();

	for (i_we = 0;i_we < n_we;i_we ++) {
		//cerr << "=" << i_we << endl;
		add((*we)[i_we]);
	}
}
/**
	 Dump a Lattice
 */
ostream& operator << (ostream &os, const Lattice &w)
{
	int i,n;

	if (!!w.we) {
		n = w.we->size();
		for (i = 0;i < n;i ++)
			os << (*w.we)[i] << endl;
	}
	/*
	n = w.wi.size();
	for (i = 0;i < n;i ++) {
		int ii,nn = w.wi[i].we.size();
		if (nn) cerr << ">" << i << " ";
		for (ii = 0;ii < nn;ii ++)
			cerr << w.wi[i].we[ii] << " ";
		if (nn)
			cerr << endl;
		nn = w.wi[i].fuzzy_map.size();
		if (nn) cerr << "<" << i << " ";
		for (ii = 0;ii < nn;ii ++)
			cerr << w.wi[i].fuzzy_map[ii] << " ";
		if (nn)
			cerr << endl;
	}
	*/
	/*
	int i, nn = w.get_word_count();
	for (i = 0;i < nn;i ++) {
		int nnn = w.get_len(i);
		for (int ii = 0;ii < nnn;ii ++) {
			int nnnn = w.get_fuzzy_count(i,ii);
			if (w.get_we_exact(i,ii))
				os << *w.get_we_exact(i,ii) << endl;
			for (int iii = 0;iii < nnnn;iii ++)
				os << w.get_we_fuzzy(i,ii,iii) << endl;
		}
	}
	*/
	return os;
}

Lattice::~Lattice()
{
	/*
	int pos,nr_pos = get_word_count();
	for (pos = 0;pos < nr_pos;pos ++) {
		int len,nr_len = get_len(pos);
		for (len = 0;len < nr_len;len ++)
			delete (*(*this)[pos])[len];
		delete (*this)[pos];
	}
	*/
}

std::ostream& operator << (std::ostream &os,const WordEntry &we)
{
	using namespace boost;
	os << format("%d %d %x %d") % we.pos % we.len % we.fuzid % we.id << we.node;
	return os;
}


/**
	 Construct a Lattice based on another Lattice. 
	 Keep only exact matches.
 */

void Lattice::based_on(const Lattice &w)
{
	vector<WordInfos> &me = wi;
	we = w.we;
	st = w.st;

	me.resize(w.get_word_count());
	int i,n = we->size();
	for (i = 0;i < n;i ++)
		if ((*we)[i].fuzid == 0)
			add((*we)[i]);
}

void Lattice::add(WordEntry &w)
{
	vector<WordInfos> &me = wi;
	if (me.size() <= w.pos)
		me.resize(w.pos+1);

	if (w.fuzid) {
		for (unsigned int j = 0;j < w.len;j ++)
			if (w.fuzid & (1 << j)) {
				if (me.size() <= j+w.pos)
					me.resize(j+w.pos+1);
				me[j+w.pos].fuzzy_map.push_back(&w);
			}
	}

	WordInfos &wis = me[w.pos];	
	wis.we.push_back(&w);
}

WordInfos::WordInfos()
{
}

unsigned int Lattice::get_len(unsigned int p) const
{
	const WordEntryRefs &we = get_we(p);
	return we.size();
}

void apply_separator(std::set<WordEntry> &wes,int i)
{
	set<WordEntry>::iterator iter;
	//cerr << "Separator applying after " << i << endl;
	for (iter = wes.begin();iter != wes.end(); ++iter) {
		//cerr << *iter << endl;
		if (iter->pos <= i) {
			if (iter->pos+iter->len-1 >= i+1)	{ // a word that across the separator
				//cerr << "Remove " << *iter << endl;
				wes.erase(iter);
			}
		} else
				break;								// because it's sorted, we don't need  to go farther
	}
}
