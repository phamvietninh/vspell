#include "config.h"							// -*- tab-width: 2 -*-
#include "wfst.h"
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/format.hpp>
#include <set>
#include "posgen.h"
#include <signal.h>

using namespace std;

int ngram_length = 2;						// for lazy men ;)

/**
	 Don't know how to name this class. 
	 It is used to generate a "to hop" ;)
 */


class ZZZ
{
private:
	std::vector<int> limit,iter;
	int nr_limit,cur;

public:
	void init(const std::vector<int> &limit);
	bool step(std::vector<int> &pos);
	void done();
};

/*
	Create a "template" segmentation. Mark all sections in template to sects
 */
/*
void WFST::create_base_segmentation(const Words &words,
																		Sections &sects,
																		Segmentation &seg)
{
	int i,ii,n,nn;
	i = ii = 0;
	n = words.get_word_count();
	nn = sects.size();

	while (i < n)
		if (ii < nn && sects[ii].start == i) {
			seg.items.push_back(Segmentation::Item());
			sects[ii].segment = seg.items.size()-1;
			ii ++;
			i += sects[ii].len;
		} else {
			// only word(i,0,0) exists
			seg.items.push_back(Segmentation::Item());
			seg.items.back().pos = i;
			i ++;
		}
}
*/

void Generator::init(const Sentence &_st)
{
  nr_misspelled = 3;		// REMEMBER THIS NUMBER
	misspelled_pos.resize(nr_misspelled);
  run = true;

	st = &(Sentence&)_st;

	len = st->get_syllable_count();

  // initialize
  for (i = 0;i < nr_misspelled;i ++)
    misspelled_pos[i] = i;

  i = 0;
}

void Generator::done()
{
}

/**
	Generate every possible 3-misspelled-positions. 
	Then call WFST::generate_misspelled_words.
 */

bool Generator::step(vector<int> &_pos,int &_len)
{
	while (run) {

		// move to the next counter
		if (i+1 < nr_misspelled && misspelled_pos[i] < len) {
			i ++;

			// do something here with misspelled_pos[i]
			_pos = misspelled_pos;
			_len = i;
			return true;
		}

		// the last counter is not full
		if (misspelled_pos[i] < len) {
			// do something here with misspelled_pos[nr_misspelled]
			_pos = misspelled_pos;
			_len = nr_misspelled;
			misspelled_pos[i] ++;
			return true;
		}

		// the last counter is full, step back
		while (i >= 0 && misspelled_pos[i] == len) i --;
		if (i < 0) 
			run = false;
		else {
			misspelled_pos[i] ++;
			for (ii = i+1;ii < nr_misspelled;ii ++)
				misspelled_pos[ii] = misspelled_pos[ii-1]+1;
		}
	}
	return false;
}

typedef vector<Segmentations> Segmentation2;

/**
	Generate new Words info based on misspelled position info.
	Call WFST::get_sections() to split into sections
	then call WFST::segment_all1() to do real segmentation.
	\param pos contains possibly misspelled position.
	\param len specify the actual length pos. Don't use pos.size()
*/
void WFST::generate_misspelled_words(const vector<int> &pos,int len)
{
	const Words &words = *p_words;
	Words w;

	w.based_on(words);
	
	// 2. Compute the score, jump to 1. if score is too low (pruning 1)

	// create new (more compact) Words structure
	int i,j,n = p_st->get_syllable_count();
	for (i = 0;i < len;i ++) {
		const WordEntryRefs &fmap = words.get_fuzzy_map(pos[i]);
		WordEntryRefs::const_iterator iter;
		for (iter = fmap.begin();iter != fmap.end(); ++iter) {
			if (*iter == 0) kill(0,SIGSTOP);
			w.add(**iter);
		}
	}

	//cerr << w << endl;

	// 4. Call get_sections
	Sections sects;
	sects.construct(words);

	// 5. Call create_base_segmentation
	//Segmentation base_seg;
	//create_base_segmentation(words,sects,base_seg);
	Segmentation2 seg2;
	seg2.resize(sects.size());

	// 6. Call segment_all1 for each sections.
	n = sects.size();
	for (i = 0;i < n;i ++) {
		Segmentation seg;
		Segmentor segtor;

		segtor.init(*p_st,					// Sentence
								w,							// Words
								sects[i].start, // from
								sects[i].start+sects[i].len-1);	// to

		while (segtor.step(seg)) {
			// compute ngram. take the best seg.
			seg.prob = 0;
			VocabIndex *vi = new VocabIndex[ngram_length];
			vi[ngram_length] = Vocab_None;
			for (int ii = ngram_length-1;ii < seg.size();ii ++) {
				for (int j = 0;j < ngram_length-1;j++)
					vi[j] = seg[ii-1-j].node.node->get_id();
				seg.prob += -ngram.wordProb(seg[ii].node.node->get_id(),vi);
			}

			seg2[i/*sects[i].segment*/].push_back(seg); // need sort

			//cout << seg << " " << seg.prob << endl;
		}

		vector<int> limits;
		vector<int> vals;
		limits.resize(sects.size());
		for (int i = 0;i < sects.size();i ++)
			limits[i] = seg2[i].size();
		ZZZ z;
		z.init(limits);

		while (z.step(vals)) {
			// merge seg to base seg.
			seg.clear();
			for (int ii = 0;ii < vals.size();ii ++) {
				//cerr << ii << " " << vals[ii] << endl;
				copy(seg2[ii][vals[ii]].begin(),
						 seg2[ii][vals[ii]].end(),
						 back_insert_iterator< Segmentation >(seg));
			}

			cout << seg << " " << seg.prob << endl;
			// 6.1. Recompute the score after each section processed. (pruning 2)
		}
		z.done();
		
	}


}

/**
	Create the best segmentation for a sentence.
	\param _sent specify the sentence
	\param words store Words info
	\param seps return the best segmentation
 */

void WFST::segment_best(const Sentence &_sent,
												const Words &words,
												Segmentation &seps)
{
	int i,ii,n,nn;

	p_st = &_sent;
	p_words = &words;

	// in test mode, generate all positions where misspelled can appear, 
	// then create a new Words for them, re get_sections, 
	// create_base_segmentation and call segment_all1 for each sections.

	// 1. Generate mispelled positions (pruning 0 - GA)
	// 2. Compute the score, jump to 1. if score is too low (pruning 1)
	// 3. Make a new Words based on the original Words
	// 4. Call get_sections
	// 5. Call create_base_segmentation
	// 6. Call segment_all1 for each sections.
	// 6.1. Recompute the score after each section processed. (pruning 2)

	// 1. Bai toan hoan vi, tinh chap C(k,n) with modification. C(1,n)+C(2,n)+...+C(k,n)
	Generator gen;

	gen.init(_sent);
	vector<int> pos;
	int len;
	while (gen.step(pos,len)) {
		//cerr << "POS :";
		//for (int i = 0;i < len;i ++) cerr << pos[i];
		//cerr << endl;
		generate_misspelled_words(pos,len);
	}
	gen.done();

}

// WFST (Weighted Finite State Transducer) implementation
// TUNE: think about genetic/greedy. Vietnamese is almost one-syllable words..
// we find where is likely a start of a word, then try to construct word
// and check if others are still valid words.

// the last item is always the incompleted item. We will try to complete
// a word from the item. If we reach the end of sentence, we will remove it
// from segs

// obsolete
void WFST::segment_all(const Sentence &sent,vector<Segmentation> &result)
{
	Words words;
	words.construct(sent);
	//	segment_all1(sent,words,0,sent.get_syllable_count(),result);a
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

/**
 * Segmentation comparator
 */

class SegmentationComparator
{
public:
	bool operator() (const Segmentation &seg1,const Segmentation &seg2) {
		return seg1.prob > seg2.prob;
	}
};






void Segmentor::init(const Sentence &sent,
										 Words &words,
										 int from,
										 int to)
{
	nr_syllables = to+1;

	segs.clear();
	segs.reserve(100);

	Trace trace;
	trace.next_syllable = from;
	trace.s.we = words.we;
	segs.push_back(trace);	// empty seg

	_sent = &(Sentence&)sent;
	_words = &words;
}

bool Segmentor::step(Segmentation &result)
{
	Sentence &sent = *_sent;
	Words &words = *_words;
	while (!segs.empty()) {
		// get one
		Trace trace = segs.back();
		segs.pop_back();

		Segmentation seg = trace.s;
		int next_syllable = trace.next_syllable;

		// segmentation completed. throw it away
		if (next_syllable >= nr_syllables)
			continue;

		//WordEntries::iterator i_entry;
		WordEntryRefs &wes = words.get_we(next_syllable);
		int ii,nn = wes.size();
		for (ii = 0;ii < nn;ii ++) {
			WordEntryRef i_entry = wes[ii];

			// New segmentation for longer incomplete word
			Trace newtrace;
			newtrace = trace;
			newtrace.s.push_back(i_entry->id);
			newtrace.s.prob += i_entry->node.node->get_prob();

			/*-it's better to compute ngram outside this function
			if (ngram_enabled) {
				if (newtrace.s.size() >= ngram_length) {
					VocabIndex *vi = new VocabIndex[ngram_length];
					vi[0] = newtrace.s.items[len-1].node(sent).node->get_id();
					vi[1] = Vocab_None;
					newtrace.s.prob += -ngram.wordProb(newtrace.s.items[len-2].node(sent).node->get_id(),vi);
					delete[] vi;
				}
			}
			*/

			newtrace.next_syllable += i_entry->len;
			if (newtrace.next_syllable == nr_syllables) {
				//cout << count << " " << newtrace.s << endl;
				result = newtrace.s;
				return true;
				//result.push_back(newtrace.s);
				//push_heap(result.begin(),result.end(),SegmentationComparator());
				//count ++;
			} else {
				segs.push_back(newtrace);
			}
		}
	} // end while
	return false;
}

void Segmentor::done()
{
}

void ZZZ::init(const vector<int> &_limit)
{
	limit = _limit;
	nr_limit = limit.size();
	iter.resize(nr_limit);
	cur = 0;
}

void ZZZ::done()
{
}

/**
	Generate every possible 3-misspelled-positions. 
	Then call WFST::generate_misspelled_words.
 */

bool ZZZ::step(vector<int> &_pos)
{
	while (cur >= 0) {

		if (cur == nr_limit) {
			_pos = iter;
			cur --;
			return true;
		}

		if (iter[cur] < limit[cur]-1) {
			iter[cur] ++;
			cur ++;
			iter[cur] = 0;
			continue;
		} else {
			cur --;
		}
	}
	return false;
}

