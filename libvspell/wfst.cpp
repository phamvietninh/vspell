#include "config.h" // -*- tab-width: 2 -*-
#include "wfst.h"
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;


void Sentence::standardize()
{
}

void Sentence::tokenize_punctuation(const string &s,vector<string> &ret)
{
	int npos,pos = 0,start = 0;
	int len = s.size();
	while (start < len) {
		if (pos < len) {
			npos = s.find_first_of("!#()'\";:.,?/",pos);
			if (npos == string::npos)
				npos = len;
			pos = npos+1;
			if (npos < len) {		// TODO: some checks here
				// floating point
				if ((s[npos] == '.' || s[npos] == ',') &&
						(npos+1 < len && s[npos+1] >= '0' && s[npos+1] <= '9'))
					continue;			// skip the dot/comma

				// date
				if ((s[npos] == '/') &&
						(npos+1 < len && s[npos+1] >= '0' && s[npos+1] <= '9'))
					continue;

				// only split dot when it's in the end.
				if (s[npos] == '.' && npos+1 != len)
					continue;
			}
		} else
			npos = len;

		ret.push_back(s.substr(start,npos-start));
		if (npos < len)
			ret.push_back(s.substr(npos,1));
		start = npos+1;
	}
}

void Sentence::tokenize()
{
	string &st = sent_;
	int pos = 0,len = st.size();
	bool started = false;		// we have found start of a new word
	Syllable sy;
	sy.span = 1;
	sy.sent_ = this;
	sy.sid = -1;

	while (pos < len) {
		if (started) {
			pos = st.find(' ',sy.start);
			if (pos == string::npos)
				pos = len;
			vector<string> ss;
			int i,n;
			tokenize_punctuation(st.substr(sy.start,pos-sy.start),ss);
			n = ss.size();
			for (i = 0;i < n;i ++) {
				string &s = ss[i];
				sy.id = sarch[s];
				transform(s.begin(),s.end(),s.begin(),viet_tolower);
				sy.cid = sarch[s];
				syllables.push_back(sy);
				sy.start += s.size();
			}
			started = false;
		} else {
			if (st[pos] == ' ')
				pos++;
			else {
				started = true;
				sy.start = pos;
			}
		}
	}
}

/*
	std::string& Sentence::const_iterator::operator++()
	{
	}

	std::string Sentence::const_iterator::operator++(int)
	{
	}
*/

class SegmentationComparator
{
public:
	bool operator() (const Segmentation &seg1,const Segmentation &seg2) {
		return seg1.prob > seg2.prob;
	}
};

void WFST::segment_best(const Sentence &_sent,
												const Words &words,
												Segmentation &seps)
{
	vector<Segmentation> segms;

	// Try to split _sent into small parts
	// mark all multiple-syllable words
	// those leaved unmarked are obviously word-segmented correctly
	vector<int> marks(_sent.get_syllable_count());
	int i,ii,n,nn;
	n = words.get_word_count();
	for (i = 0;i < n;i ++) {
		nn = words.get_len(i);
		if (nn > 1)
			for (ii = 0;ii < nn;ii ++) 
				marks[i+ii] = 1;
	}

	/*
		for (i = 0;i < marks.size();i ++)
		if (marks[i])
		cout << sarch[_sent[i].get_id()] << " ";
		else
		cout << "*" << sarch[_sent[i].get_id()] << "* ";
		cout << endl;
	*/

	// "tokenize" _sent
	int pos = 0,len = marks.size();
	bool started = false;
	int start = 0;

	seps.items.reserve(20);

	while (pos < len) {
		if (!marks[pos]) {
			seps.items.push_back(Segmentation::Item());
			Segmentation::Item &item = seps.items.back();
			item.state = words.get_fuzzy(pos,0,0).node;
			item.distance = 0/*FIXME*/;
			item.flags |= SEGM_SEPARATOR;
			seps.prob += item.state->get_prob();
			int len = seps.items.size();
			if (len > 1 && ngram_enabled) {
				VocabIndex vi[2];
				vi[0] = seps.items[len-1].state->get_id();
				vi[1] = Vocab_None;
				seps.prob += -ngram.wordProb(seps.items[len-2].state->get_id(),vi);
			}
			seps.distance += item.distance;
			pos++;
		} else {
			start = pos;
			while (pos < len && marks[pos] == 1) pos ++;
			segms.clear();
			segment_all1(_sent,words,start,pos,segms);
			if (!segms.empty()) {
				int len = seps.items.size();
				if (len > 0 && ngram_enabled) {
					VocabIndex vi[2];
					vi[0] = segms[0].items[0].state->get_id();
					vi[1] = Vocab_None;
					seps.prob += -ngram.wordProb(seps.items[len-1].state->get_id(),vi);
				}
				seps.items.insert(seps.items.end(),
													segms[0].items.begin(),
													segms[0].items.end());
				seps.prob += segms[0].prob;
				seps.distance += segms[0].distance;
			}
		}
	}

	/*
		vector<Segmentation>::iterator i;
		for (i = segms.begin();i != segms.end(); ++i)
		i->print(_sent);
	*/
}


// find all possible words
// words must be cleared before calling this function
void WFST::get_all_words(const Sentence &sent,Words &words)
{
	int i,n,ii,nn,k;
	vector<WordNode::DistanceNode> states,old_states;
	vector<strid> syll;

	states.reserve(10);
	old_states.reserve(10);

	n = sent.get_syllable_count();

	syll.resize(n);
	for (i = 0;i < n;i ++)
		syll[i] = sent[i].get_cid();

	for (i = 0;i < n;i ++) {

		// make a word
		words.push_back(new WordInfos());
		WordInfos &winfos = *words.back();

		// finding all possible words started by the i-th syllable.

		// init states with root node
		states.clear();
		states.push_back(WordNode::DistanceNode(wl));

		k = 0;
		while (!states.empty() && i+k < n) {

			// make a winfo
			winfos.push_back(new WordInfo());
			WordInfo &winfo = *winfos.back();

			// save states
			old_states = states;

			// clear states, ready for new states
			states.clear();

			// get next new states -> states
			nn = old_states.size();
			for (ii = 0;ii < nn;ii ++) {
				WordNode::DistanceNode &state = old_states[ii];
				WordNodePtr exact_node = state.node->get_next(syll[i+k]);
				if (exact_node) states.push_back(exact_node);
				state.node->fuzzy_get_next(syll[i+k],states);
			}

			// fill winfo
			nn = states.size();
			for (ii = 0;ii < nn;ii ++)
				if (states[ii].node->get_prob() >= 0) {
					winfo.fuzzy_match.push_back(states[ii]);
				}

			// unknown word
			if (k == 0 && winfo.fuzzy_match.empty()) {
				WordNode::DistanceNode dn;
				dn.node = wl->get_next(unk_id);
				dn.distance = 0;
				winfo.fuzzy_match.push_back(dn);
			}

			// remove duplicated items
			sort(winfo.fuzzy_match.begin(),winfo.fuzzy_match.end());
			winfo.fuzzy_match.erase(unique(winfo.fuzzy_match.begin(),
																		 winfo.fuzzy_match.end()),
															winfo.fuzzy_match.end());

			k ++;
		}
		// remove the last if it is empty
		while (!winfos.empty() && winfos.back()->fuzzy_match.empty())
			winfos.pop_back();
	}

}

void Words::print() const
{
	int i, nn = get_word_count();
	for (i = 0;i < nn;i ++) {
		int nnn = get_len(i);
		cerr << "From " << i << endl;
		for (int ii = 0;ii < nnn;ii ++) {
			int nnnn = get_fuzzy_count(i,ii);
			cerr << "Len " << ii << endl;
			for (int iii = 0;iii < nnnn;iii ++) {
				cerr << get_fuzzy(i,ii,iii).node->get_id( ) << "-" << sarch[get_fuzzy(i,ii,iii).node->get_id()] << " ";
				cerr << get_fuzzy(i,ii,iii).node->get_syllable_count() << " ";
				cerr << get_fuzzy(i,ii,iii).distance << " ";
				cerr << get_fuzzy(i,ii,iii).node->get_prob() << endl;
			}
		}
	}
}

// WFST (Weighted Finite State Transducer) implementation
// TUNE: think about genetic/greedy. Vietnamese is almost one-syllable words..
// we find where is likely a start of a word, then try to construct word
// and check if others are still valid words.

// the last item is always the incompleted item. We will try to complete
// a word from the item. If we reach the end of sentence, we will remove it
// from segs

struct Trace
{
	Segmentation s;
	int next_syllable;
	Trace():next_syllable(0) {}
};

void WFST::segment_all(const Sentence &sent,vector<Segmentation> &result)
{
	Words words;
	get_all_words(sent,words);
	segment_all1(sent,words,0,sent.get_syllable_count(),result);
	/*
		int nn = words.size();
		for (i = 0;i < nn;i ++) {
		int nnn = words[i].size();
		cerr << "From " << i << endl;
		for (int ii = 0;ii < nnn;ii ++) {
		int nnnn = words[i][ii].fuzzy_match.size();
		cerr << "Len " << ii << endl;
		for (int iii = 0;iii < nnnn;iii ++) {
		cerr << words[i][ii].fuzzy_match[iii].distance << " ";
		cerr << words[i][ii].fuzzy_match[iii].node->get_prob() << endl;
		}
		}
		}
	*/

}

void WFST::segment_all1(const Sentence &sent,
												const Words &words,
												int from,int to,
												std::vector<Segmentation> &result)
{
	int nr_syllables = to;
	vector<Trace> segs;		// used to store all incomplete seg
	int i;
	int count = 0;
	Trace trace;

	trace.next_syllable = from;
	//Segmentation start_seg;
	segs.reserve(100);
	segs.push_back(trace);	// empty seg

	while (!segs.empty()) {
		// get one
		trace = segs.back();
		segs.pop_back();
		Segmentation seg = trace.s;
		int next_syllable = trace.next_syllable;
//		cerr << segs.size() << " " << count << endl;

		// segmentation completed. throw it away
		// TUNE: do we need these? -> yes we do
		// in case of bad/incomplete segmentations
		if (next_syllable == nr_syllables)
			continue;

		int nr_size_1 = words.get_len(next_syllable);

		// word with the length of (size_1+1) syllables
		for (int size_1 = 0;size_1 < nr_size_1; size_1 ++) {
			int nr_fuzzy = words.get_fuzzy_count(next_syllable,size_1);

			// number of words
			for (i = 0;i < nr_fuzzy;i ++) {
				const WordNode::DistanceNode &next_state = 
					words.get_fuzzy(next_syllable,size_1,i);

				// New segmentation for longer incomplete word
				Trace newseg;
				newseg = trace;
				newseg.s.items.push_back(Segmentation::Item());
				Segmentation::Item &item = newseg.s.items.back();
				item.state = next_state.node; // move to the next node
				item.distance = 0/*FIXME*/;
				item.flags |= SEGM_SEPARATOR;

				// Segmentation's parameters
				newseg.s.distance += item.distance;
				newseg.s.prob += item.state->get_prob();
				int len = newseg.s.items.size();
				if (len > 1 && ngram_enabled) {
					VocabIndex vi[2];
					vi[0] = newseg.s.items[len-1].state->get_id();
					vi[1] = Vocab_None;
					newseg.s.prob += -ngram.wordProb(newseg.s.items[len-2].state->get_id(),vi);
				}
				newseg.next_syllable += size_1+1;
				if (newseg.next_syllable == nr_syllables) {
					result.push_back(newseg.s);
					push_heap(result.begin(),result.end(),SegmentationComparator());
					//cerr << "New: ";newseg.s.print(sent);
					//cerr << "Candidate: ";result[0].print(sent);
					count ++;
				} else {
					segs.push_back(newseg);
					//cerr << "New seg: ";
					//newseg.print(sent);
				}
			}
		}
	} // end while
}


void Segmentation::print(ostream &os, const Sentence &st)
{
	int cc,i,n = items.size();
	/*
		for (i = 0;i < n;i ++) {
		cout << *st[i] << " ";
		if (items[i].flags & SEGM_SEPARATOR)
		cout << "| ";
		}
		cout << endl;
	*/
	for (cc = i = 0;i < n;i ++) {
		int c = items[i].state->get_syllable_count();
		for (int k = 0;k < c;k ++) {
			if (k) os << "_";
			os << sarch[st[cc+k].get_id()];
		}
		cc += c;
		os << " ";
	}
	os << prob << endl;
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
