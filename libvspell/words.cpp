#include "config.h"							// -*- tab-width: 2 -*-
#include "spell.h"
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/format.hpp>
#include <set>
#include "propername.h"


using namespace std;


/**
	Store information used by WFST::get_all_words()
*/
struct WordState {

	/**
		 the currently processing node
	 */
	WordNode::DistanceNode dnode;

	int fuzid;
	int pos;

	WordState(const WordNode::DistanceNode &n):dnode(n),fuzid(0) {}
};

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
	pre_construct(sent,wes);
	mark_proper_name(sent,wes);
	post_construct(wes);
}

/**
	The first phase of lattice creation. Create all possible words to we.
 */

void Lattice::pre_construct(const Sentence &sent,set<WordEntry> &we)
{
	Lattice &w = *this;
	int i,n,ii,nn,k,nnn,iii;

	//cerr << "construct\n";

	w.st = &sent;

	vector<WordState> states1;
	vector<WordState> states2;
	states1.reserve(10);
	states2.reserve(10);

	n = sent.get_syllable_count();

	for (i = 0;i < n;i ++) {

		states1.push_back(WordNode::DistanceNode(get_root()));
		states1.back().pos = i;
		states2.clear();

		nn = states1.size();
		for (ii = 0;ii < nn;ii ++) {
			const WordState &ws = states1[ii];

			WordNodePtr exact_node = ws.dnode.node->get_next(sent[i].get_cid());
			WordNodePtr lowercase_node = ws.dnode.node->get_next(get_sarch()[get_lowercased_syllable(get_sarch()[sent[i].get_cid()])]);
			vector<WordNode::DistanceNode> nodes;
			ws.dnode.node->fuzzy_get_next(sent[i].get_cid(),nodes);
			if (exact_node && 
					find(nodes.begin(),nodes.end(),exact_node) == nodes.end())
				nodes.push_back(exact_node);
			if (lowercase_node && 
					find(nodes.begin(),nodes.end(),lowercase_node) == nodes.end())
				nodes.push_back(lowercase_node);

			nnn = nodes.size();
			for (iii = 0;iii < nnn;iii ++) {
				states2.push_back(ws);	// inherit from the current state
				WordState &ws2 = states2.back();
				ws2.dnode = nodes[iii];
				if (nodes[iii].node != exact_node && nodes[iii].node != lowercase_node)
					ws2.fuzid |=  1 << k;
				
				// get completed words
				if (ws2.dnode.node->get_prob() >= 0) {
					WordEntry e;
					e.pos = ws2.pos;
					e.len = i-ws2.pos+1;
					e.fuzid = ws2.fuzid;
					e.node = ws2.dnode;
					we.insert(e);
				}
			}
		}
		
		states1.swap(states2);
	}
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
				e.node = get_root()->get_next(get_id(NUMBER_ID));
			else
				e.node = get_root()->get_next(get_id(UNK_ID));
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

	for (i_we = 0;i_we < n_we;i_we ++)
		add((*we)[i_we]);
}
/**
	 Dump a Lattice
 */
ostream& operator << (ostream &os, const Lattice &w)
{
	int i,n = w.we->size();
	for (i = 0;i < n;i ++)
		os << (*w.we)[i] << endl;
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
	os << format("%d %d %x ") % we.pos % we.len % we.fuzid << we.node;
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

	WordInfos &wis = me[w.pos];

	if (w.fuzid) {
		for (unsigned int j = 0;j < w.len;j ++)
			if (w.fuzid & (1 << j)) {
				if (me.size() <= j+w.pos)
					me.resize(j+w.pos+1);
				me[j+w.pos].fuzzy_map.push_back(&w);
			}
	}

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
