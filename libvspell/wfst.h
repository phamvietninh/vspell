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

struct Section {
	int segment;
	int start;
	int len;
	void segment_best(const Words&,Segmentation &final_seg);
};

class Sections: public std::vector<Section> {
public:
	Sentence const * st;					/// This is used for operator<< only
	void construct(const Words &words);
	friend std::ostream& operator << (std::ostream &os,const Sections &s);
};


/**
	 Segmentor takes a Sentence, a Words and a range, then try to generate
	 all possible Segmentation.
 */

class Segmentor
{
private:
	struct Trace
	{
		Segmentation s;
		int next_syllable;
		Trace(boost::shared_ptr<WordEntries> _we):next_syllable(0),s(_we) {}
	};
	int nr_syllables;
	std::vector<Trace> segs;
	Words const *_words;
	int from,to;

public:
	void init(const Words &words,
						int from,
						int to);
	bool step(Segmentation &seg);
	void done();
};

class WFST
{
protected:
	WordNodePtr wl;  
	bool ngram_enabled;

public:
	WFST():wl(NULL),ngram_enabled(false) {}

	bool set_wordlist(WordNodePtr _wl) {
		wl = _wl;
		return true;
	}

	void enable_ngram(bool enable = true) { ngram_enabled = enable; }

	void segment_best(const Words &words,Segmentation &seps);
	void segment_best_no_fuzzy(const Words &words,Segmentation &seps);
	void segment_all(const Sentence &sent,std::vector<Segmentation> &result);

	//private:
public:													// for testing purpose
	void generate_misspelled_words(const std::vector<int> &pos,
																 int len,
																 Segmentation& final_seg);
	// variables needed when run wfst
	Words const *p_words;
};

#endif
