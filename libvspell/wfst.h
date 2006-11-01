#ifndef __WFST_H__ // -*- tab-width: 2 mode: c++ -*-
#define __WFST_H__

#ifndef __DICTIONARY_H__
#include "dictionary.h"
#endif

#ifndef __WORDNODE_H__
#include "wordnode.h"
#endif

#ifndef __SPELL_H__
#include "spell.h"
#endif

#ifndef __VECTOR__
#include <vector>
#endif

#ifndef BOOST_SHARED_PTR_HPP_INCLUDED
#include <boost/shared_ptr.hpp>
#endif

#define SEGM_SEPARATOR 1
#define NGRAM_LENGTH 2

struct Section {
	uint segment;
	uint start;
	uint len;
	void segment_best(const Lattice&,Segmentation &final_seg);
};

class Sections: public std::vector<Section> {
public:
	Sentence const * st;					/// This is used for operator<< only
	void construct(const Lattice &words);
	friend std::ostream& operator << (std::ostream &os,const Sections &s);
};


/**
	 Segmentor takes a Sentence, a Lattice and a range, then try to generate
	 all possible Segmentation.
 */

class Segmentor
{
private:
	struct Trace
	{
		Segmentation s;
		int next_syllable;
		Trace(boost::shared_ptr<WordEntries> _we):s(_we),next_syllable(0) {}
	};
	int nr_syllables;
	std::vector<Trace> segs;
	Lattice const *_words;
	int from,to;

public:
	void init(const Lattice &words,
						int from,
						int to);
	bool step(Segmentation &seg);
	void done();
};

class WFST
{
protected:
	bool ngram_enabled;

public:
	WFST():ngram_enabled(false) {}

	void enable_ngram(bool enable = true) { ngram_enabled = enable; }

	void segment_best(const Lattice &words,Segmentation &seps);
	void segment_best_no_fuzzy(const Lattice &words,Segmentation &seps);
	void segment_all(const Sentence &sent,std::vector<Segmentation> &result);

	//private:
public:													// for testing purpose
	void generate_misspelled_words(const std::vector<uint> &pos,
																 int len,
																 Segmentation& final_seg);
	// variables needed when run wfst
	Lattice const *p_words;
};

#endif
