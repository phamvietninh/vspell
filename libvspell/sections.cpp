#include "config.h"							// -*- tab-width: 2 -*-
#include "wfst.h"
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/format.hpp>
#include <set>


using namespace std;


/**
 * dump a Sections
 */

std::ostream& operator << (std::ostream &os,const Sections &me)
{
	using namespace boost;
	const Sentence &_sent = *me.st;
	unsigned int i,ii,nn,n = _sent.get_syllable_count();
	ii = 0;
	nn = me.size();
	for (i = 0;i < n;i ++) {
		if (ii < nn && me[ii].start == i)
			os << "[";
		os << format("%s") % get_sarch()[_sent[i].id];
		if (ii < nn && me[ii].start+me[ii].len-1 == i) {
			os << "]" << me[ii].len;
			ii ++;
		}
		os << " ";
	}
	return os;
}

/**
 Split Lattice into Sections
 \param words input
 \param sects output
 */

void Sections::construct(const Lattice &words)
{
	Sections& sects = *this;

	// mark all possible words. All bounds left is really bounds.
	// because we need at most n-1 boundary syllable for n-gram
	// if two ambiguous sections is near than n-1 syllables, then merge them.
	unsigned int i,ii,n,nn;

	sects.st = words.st;

	n = words.get_word_count();

	vector<uint> bound(n);

	for (i = 0;i < n;i ++) {
		nn = words.get_len(i);
		for (ii = 0;ii < nn-2;ii ++)
			bound[ii+i] = 1;
	}
	if (!bound.size())
		return;
	bound[bound.size()-1] = 1; // it's obvious there is a boundary in the end

	//copy(bound.begin(),bound.end(),ostream_iterator<int>(cerr," "));

	// "tokenize" _sent
	int pos,len = bound.size();
	Section sect;

	sect.start = 0;
	sect.len = 0;

	for (pos = 0;pos < len;pos ++) {
		// ignore "1" boundaries
		if (bound[pos])
			continue;

		bool is_section;
		// just write down and figure out what the formulas mean
		sect.len = pos - sect.start + 1;
		is_section = words.get_len(sect.start) > 2 || 
			(words.get_len(sect.start) >= 2 && 
			 !words.get_fuzzy_map(sect.start).empty());

		if (is_section) {
			// now merge two sections (this and the previous) if needed
			if (!sects.empty()) {
				Section &prev = sects.back();
				if (sect.start - (prev.start + prev.len) < NGRAM_LENGTH-1)
					prev.len = pos - prev.start + 1; // merge
				else
					sects.push_back(sect); // not merge
			} else
				sects.push_back(sect);	// sects is empty -> nothing to merge
		}
		sect.start = pos+1;
	}

	if (sects.empty()) {
		sect.start=0;
		sect.len = n-sect.start;
		sects.push_back(sect);
	}
}

std::ostream& operator <<(std::ostream &os,const Segmentation &seg)
{
           int i,n = seg.size();
	for (i = 0;i < n;i ++)
	            	os << "[" << seg[i].node << "] ";
	return os;
}

std::ostream& Segmentation::pretty_print(std::ostream &os,const Sentence &st)
{
	int i,n = size();
	VocabIndex id;
	for (i = 0;i < n;i ++) {
		if (i)
			os << " ";
		id = (*this)[i].node.node->get_id();
		if (id == get_id(UNK_ID))
			id = st[(*this)[i].pos].id;
		os << get_sarch()[id];
	}
	return os;
}

void Section::segment_best(const Lattice &w,Segmentation &final_seg)
{
	Segmentation seg(w.we);
	Segmentor segtor;
	final_seg.prob = 1000000;

	segtor.init(w,								// Lattice
							start,						// from
							start+len-1);			// to

	VocabIndex *vi = new VocabIndex[NGRAM_LENGTH];
	while (segtor.step(seg)) {
		// compute ngram. take the best seg.
		seg.prob = 0;
		vi[NGRAM_LENGTH] = Vocab_None;
		for (unsigned int ii = NGRAM_LENGTH-1;ii < seg.size();ii ++) {
			for (unsigned int j = 0;j < NGRAM_LENGTH-1;j++)
				vi[j] = seg[ii-1-j].node.node->get_id();
			seg.prob += -get_ngram().wordProb(seg[ii].node.node->get_id(),vi);
		}
		
		if (seg.prob < final_seg.prob)
			final_seg = seg;
		
		//cerr << seg << " " << seg.prob << endl;
	}

	delete[] vi;
}
