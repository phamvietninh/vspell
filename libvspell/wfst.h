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

#define SEGM_SEPARATOR 1

struct Section {
	int segment;
	int start;
	int len;
};

class Sections: public std::vector<Section> {
public:
	SentenceRef st;
	void construct(const Words &words);
};

std::ostream& operator << (std::ostream &os,const Sections &s);

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
		Trace():next_syllable(0) {}
	};
	int nr_syllables;
	std::vector<Trace> segs;
	SentenceRef _sent;
	Words *_words;
	int from,to;

public:
	void init(const Sentence &sent,
						Words &words,
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

	void segment_best(const Sentence &sent,
										const Words &words,
										Segmentation &seps);
	void segment_all(const Sentence &sent,
			 std::vector<Segmentation> &result);

	//private:
public:													// for testing purpose
	void generate_misspelled_words(const std::vector<int> &pos,int len);
	void segment_all1(const Sentence &sent,
										Words &words,
										int from,
										int to,
										//const Segmentation &base_seg,
										//int seg_id,
										std::vector<Segmentation> &result);

	// variables needed when run wfst
	const Sentence *p_st;
	const Words *p_words;
};

#endif
