#include "config.h"							// -*- tab-width: 2 -*-
#include "spell.h"
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/format.hpp>
#include <set>


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

	WordState(const WordNode::DistanceNode &n):dnode(n),fuzid(0) {}
};

/**
	Find all possible words
	\param sent specify the input sentence
	\param w must be cleared before calling this function
*/
void Words::construct(const Sentence &sent)
{
	Words &w = *this;
	int i,n,ii,nn,k,nnn,iii;
	vector<strid> syll;
	set<WordEntry> we;

	w.st = &(Sentence&)sent;

	vector<WordState> states;
	vector<WordState> old_states(states);
	states.reserve(10);
	old_states.reserve(10);

	n = sent.get_syllable_count();

	syll.resize(n);
	for (i = 0;i < n;i ++) {
		syll[i] = sent[i].get_cid();
		//cerr << syll[i] << " ";
	}
	//cerr << endl;

	for (i = 0;i < n;i ++) {			// pos

		// finding all possible words started at the i-th syllable.

		// init states with root node
		states.clear();
		states.push_back(WordNode::DistanceNode(get_root()));

		for (k = 0;k+i < n && !states.empty(); k ++) { // length of k
			// save states
			old_states = states;

			/*
			cerr << "State:" << i << " " << k << endl;
			int jj,jnn = states.size();
			for (jj = 0;jj < jnn;jj ++) {
				cerr << sarch[states[jj].dnode.node->get_syllable()] << endl;
				if (states[jj].dnode.node != wl) {
					states[jj].dnode.node->dump_next(cerr);
					cerr << endl;
				}
			}
			*/

			// clear states, ready for new states
			states.clear();

			// get next new states -> states
			// those in old_states which has reached end will be removed from states
			nn = old_states.size();
			bool has_words = false;
			for (ii = 0;ii < nn;ii ++) {
				WordNode::DistanceNode &state = old_states[ii].dnode;
				WordNodePtr exact_node = state.node->get_next(syll[i+k]);
				vector<WordNode::DistanceNode> nodes;
				//nodes.clear();
				//if (exact_node)
				//nodes.push_back(exact_node);
				state.node->fuzzy_get_next(syll[i+k],nodes);
				if (exact_node && 
						find(nodes.begin(),nodes.end(),exact_node) == nodes.end())
					nodes.push_back(exact_node);
				nnn = nodes.size();
				for (iii = 0;iii < nnn;iii ++) {
					states.push_back(old_states[ii]);
					states.back().dnode = nodes[iii];
					if (nodes[iii].node != exact_node)
						states.back().fuzid |=  1 << k;

					// get completed words
					if (states.back().dnode.node->get_prob() >= 0) {
						WordEntry e;
						e.pos = i;
						e.len = k+1;
						e.fuzid = states.back().fuzid;
						e.node = states.back().dnode;
						we.insert(e);
						has_words = true;
						//winfo.fuzzy_match.push_back(states[ii]);
					}
				}
			}


			// this is an unknown syllable, we presume this is an unknown word
			if (k == 0 && !has_words) {
				WordEntry e;
				e.pos = i;
				e.len = 1;
				e.fuzid = 0;
				e.node = get_root()->get_next(unk_id);
				we.insert(e);
			}
		}	// end of k
	}	// end of i

	//copy(we.begin(),we.end(),ostream_iterator<WordEntry>(cerr));
	// copy to real _we
	n = we.size();
	w.we = new WordEntries;
	w.we->reserve(n);
	copy(we.begin(),we.end(),back_insert_iterator<vector<WordEntry> >(*w.we));
	for (i = 0;i < n;i ++)
		(*w.we)[i].id = i;

	// build Words structure
	w.construct();
}

/**
	 Post-process after initialize core member of Words
 */

void Words::construct()
{
	int i_we,n_we = we->size();

	for (i_we = 0;i_we < n_we;i_we ++)
		add((*we)[i_we]);
}
/**
	 Dump a Words
 */
ostream& operator << (ostream &os, const Words &w)
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

Words::~Words()
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
	 Construct a Words based on another Words. 
	 Keep only exact matches.
 */

void Words::based_on(const Words &w)
{
	Words &me = *this;
	we = w.we;
	me.st = w.st;

	int i_pos,n_pos = w.get_word_count();

	me.resize(n_pos);

	for (i_pos = 0;i_pos < n_pos;i_pos ++) {
		int i_len,n_len = w.get_len(i_pos);
		me[i_pos] = new WordInfos;
		for (i_len = 0;i_len < n_len;i_len ++)
			if ((*w[i_pos])[i_len] && 
					(*w[i_pos])[i_len]->exact_match)
				add(*(*w[i_pos])[i_len]->exact_match);
	}
}

void Words::add(WordEntry &w)
{
	Words &me = *this;
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
		for (int j = 0;j < w.len;j ++)
			if (w.fuzid & (1 << j)) {
				while (me.size() <= j+w.pos)
					me.push_back(new WordInfos);
				me[j+w.pos]->fuzzy_map.push_back(&w);
			}
	}

	wis.we.push_back(&w);
}
