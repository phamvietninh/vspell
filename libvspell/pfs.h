#ifndef __PFS_H__ // -*- tab-width: 2 mode: c++ -*-
#define __PFS_H__

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

/**
   Priority-First Search.
   This is the PFS approach to the shortest path problem in graph.
 */

class PFS
{
protected:
	WordNodePtr wl;  
	bool ngram_enabled;

public:
	PFS():wl(NULL),ngram_enabled(false) {}

	bool set_wordlist(WordNodePtr _wl) {
		wl = _wl;
		return true;
	}

	void enable_ngram(bool enable = true) { ngram_enabled = enable; }

	void segment_best(const Lattice &words,Segmentation &seps);
	void segment_best_no_fuzzy(const Lattice &words,Segmentation &seps) { segment_best(words,seps); }
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
