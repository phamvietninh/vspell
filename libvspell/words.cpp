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
	Find all possible words
	\param sent specify the input sentence
	\param w must be cleared before calling this function
*/
void Lattice::construct(const Sentence &sent)
{
	Lattice &w = *this;
	int i,n,ii,nn,k,nnn,iii;
	vector<bool> marks;
	set<WordEntry> we;

	//cerr << "construct\n";

	w.st = &sent;

	boost::scoped_ptr<vector<WordState> > states1(new vector<WordState>);
	boost::scoped_ptr<vector<WordState> > states2(new vector<WordState>);
	states1->reserve(10);
	states2->reserve(10);

	n = sent.get_syllable_count();
	marks.resize(n);

	for (i = 0;i < n;i ++) {

		states1->push_back(WordNode::DistanceNode(get_root()));
		states1->back().pos = i;
		states2->clear();

		nn = states1->size();
		bool has_words = false;
		for (ii = 0;ii < nn;ii ++) {
			const WordState &ws = (*states1)[ii];

			WordNodePtr exact_node = ws.dnode.node->get_next(sent[i].get_cid());
			WordNodePtr lowercase_node = ws.dnode.node->get_next(sarch[get_lowercased_syllable(sarch[sent[i].get_cid()])]);
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
				states2->push_back(ws);	// inherit from the current state
				WordState &ws2 = states2->back();
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
					for (int k = ws2.pos; k <= i;k ++)
						marks[k] = true;
				}
			}
		}
		
		states1.swap(states2);
	}

	mark_proper_name(sent,we);

	// make sure that one can go from source to destination, without gaps.
	for (i = 0;i < n;i ++)
		if (!marks[i]) { 
			WordEntry e;
			e.pos = i;
			e.len = 1;
			e.fuzid = 0;
			// if one starts with a cardinal number, then mark it number_id
			string s = sarch[sent[i].get_cid()];
			if (strchr("0123456789",s[0]) != NULL)
				e.node = get_root()->get_next(number_id);
			else
				e.node = get_root()->get_next(unk_id);
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
	return os;
}

Lattice::~Lattice()
{
	int pos,nr_pos = get_word_count();
	for (pos = 0;pos < nr_pos;pos ++) {
		int len,nr_len = get_len(pos);
		for (len = 0;len < nr_len;len ++)
			delete (*(*this)[pos])[len];
		delete (*this)[pos];
	}
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
	Lattice &me = *this;
	we = w.we;
	me.st = w.st;

	int i_pos,n_pos = w.get_word_count();

	me.resize(n_pos);

	for (i_pos = 0;i_pos < n_pos;i_pos ++) {
		int i_len,n_len = w.get_len(i_pos);
		me[i_pos] = new WordInfos;
		for (i_len = 0;i_len < n_len;i_len ++)
			if ((*w[i_pos])[i_len] && (*w[i_pos])[i_len]->exact_match)
				add(*(*w[i_pos])[i_len]->exact_match);
	}
}

void Lattice::add(WordEntry &w)
{
	Lattice &me = *this;
	while (me.size() <= w.pos)
		me.push_back(new WordInfos);

	WordInfos &wis = *me[w.pos];

	while (wis.size() <= w.len) {
		wis.push_back(new WordInfo);
		wis.back()->exact_match = NULL;
	}

	WordInfo &wi = *wis[w.len];
	if (!w.fuzid)								// exact match
		wi.exact_match = &w;
	else {
		wi.fuzzy_match.push_back(&w);
		for (unsigned int j = 0;j < w.len;j ++)
			if (w.fuzid & (1 << j)) {
				while (me.size() <= j+w.pos) {
					me.push_back(new WordInfos);
					//me[me.size()-1]->fuzzy_map.clear();
				}
				me[j+w.pos]->fuzzy_map.push_back(&w);
			}
	}

	wis.we.push_back(&w);
}

WordInfos::WordInfos()
{
}
