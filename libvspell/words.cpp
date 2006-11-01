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
#include <SArray.cc>
#endif


using namespace std;

WordState::WordState(const WordState &ws):dnode(ws.dnode),sent(ws.sent),fuzid(ws.fuzid),pos(ws.pos),len(ws.len)
{
}

void WordState::add_word(set<WordEntry> &we,LeafNNode*leaf)
{
	WordEntry e;
	e.pos = pos;
	e.len = len;
	e.fuzid = fuzid;
	e.node = leaf;
	//cerr << "Add " << e << endl;
	we.insert(e);
}

void WordState::collect_words(set<WordEntry> &we)
{
	LeafNNode *leaf = dnode.node->get_leaf(sarch["<mainleaf>"]);
	if (leaf)
		add_word(we,leaf);
}

void UpperWordState::collect_words(set<WordEntry> &we)
{
	LeafNNode *leaf = dnode.node->get_leaf(sarch["<caseleaf>"]);
	if (leaf)
		add_word(we,leaf);
}

void WordState::get_first(WordStates &states,uint _pos)
{
	dnode.node = warch.get_root();
	pos = _pos;
	fuzid = 0;
	len = 0;
	get_next(states);
}

void ExactWordState::get_next(WordStates &states)
{
	uint i = pos+len;
	BranchNNode *branch = dnode.node->get_branch(sent[i].get_cid());
	if (branch == NULL) {
		delete this;
		return;
	}
	//cerr << "Exact: " << get_sarch()[sent[i].get_cid()] << endl;
	states.push_back(this);
	// change the info
	dnode = branch;
	len ++;
}

void LowerWordState::get_next(WordStates &states)
{
	string s1,s2;
	uint i = pos+len;
	s1 = get_sarch()[sent[i].get_cid()];
	s2 = get_lowercased_syllable(s1);
	BranchNNode *branch = dnode.node->get_branch(get_sarch()[s2]);
	if (branch == NULL) {
		delete this;
		return;
	}
	//cerr << "Lower: " << get_lowercased_syllable(get_sarch()[sent[i].get_cid()]) << endl;
	states.push_back(this);
	// change the info
	dnode = branch;
	len ++;
	if (s1 != s2)
		fuzid |= 1 << (i-pos);
}

void FuzzyWordState::get_next(WordStates &states)
{
	vector<confusion_set>& confusion_sets = get_confusion_sets();
	int i,j,m,n = confusion_sets.size();
	bool ret = false;
	set<Syllable> syllset,syllset2;
	Syllable _syll;
	uint _i = pos+len;
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
		BranchNNode::const_np_range range = dnode.node->get_nodes().equal_range(get_sarch()[str]);
		BranchNNode::node_map::const_iterator pnode;
		for (pnode = range.first;pnode != range.second;++pnode)
			if (!pnode->second->is_leaf()) {
				//cerr << "Fuzzy: " << iter->to_std_str() << endl;
				FuzzyWordState *s = new FuzzyWordState(*this);

				// change the info
				s->dnode.node = (BranchNNode*)pnode->second.get();
				s->len++;
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
	UpperWordStateFactory upper;
	FuzzyWordStateFactory fuzzy;
	factories.push_back(&exact);
	factories.push_back(&lower);
	factories.push_back(&upper);
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
			states1[ii]->get_next(states2);

		// get completed words
		nn = states2.size();
		for (ii = 0;ii < nn;ii ++)
			states2[ii]->collect_words(we);

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

bool we_pos_cmp(const WordEntry &w1,const WordEntry &w2)
{
	return w1.pos < w2.pos;
}

void Lattice::post_construct(set<WordEntry> &we)
{
	Lattice &w = *this;
	unsigned i,n,k;

	n = st->get_syllable_count();

	uint max = 0,len = 0;
	while (max < n) {

		// find out how far we can go from head (pos -> max, len -> len)
		set<int> traces;
		pair<set<WordEntry>::iterator,set<WordEntry>::iterator> pr;
		set<WordEntry>::iterator iter;
		traces.insert(max);
		while (!traces.empty()) {
			uint ii, nn = 0;
			WordEntry fake_entry;
			fake_entry.pos = *traces.begin();
			traces.erase(traces.begin());
			pr = equal_range(we.begin(),we.end(),fake_entry,we_pos_cmp);
			for (iter = pr.first;iter != pr.second; ++iter) {
				if (iter->pos+iter->len < n)
					traces.insert(iter->pos+iter->len);
				pair<set<WordEntry>::iterator,set<WordEntry>::iterator> pr2;
				set<WordEntry>::iterator iter2;
				WordEntry fake_entry2;
				fake_entry2.pos = iter->pos+iter->len;
				pr2 = equal_range(we.begin(),we.end(),fake_entry2,we_pos_cmp);
				if (pr2.first == pr2.second && len < iter->pos+iter->len) {
					max = iter->pos;
					len = iter->pos+iter->len;
				}
			}
		}

		// Yee, we're coming tail!
		if (len >= n)
			break;

		// make sure that one can go from source to destination, without gaps.
		WordEntry e;
		e.pos = len;
		e.len = 1;
		e.fuzid = 0;
		// if one starts with a cardinal number, then mark it number_id
		string s = get_sarch()[(*st)[max].get_cid()];
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
		max = len+1;
		len = max;
		//cerr << " " << get_sarch()[e.node.node->get_id()] << endl;
		we.insert(e);
	}

	//copy(we.begin(),we.end(),ostream_iterator<WordEntry>(cerr));
	// copy to real _we
	n = we.size();
	w.we = boost::shared_ptr<WordEntries>(new WordEntries(n));
	//w.we->resize(n);
	copy(we.begin(),we.end(),w.we->begin());
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
