#ifndef __SOFTCOUNT_H__ // -*- tab-width: 2 mode: c++ -*-
#define __SOFTCOUNT_H__

#ifndef __DICTIONARY_H__
#include "dictionary.h"
#endif

#ifndef __WORDNODE_H__
#include "wordnode.h"
#endif

#ifndef __SPELL_H__
#include "spell.h"
#endif

#ifndef __DAG_H__
#include "dag.h"
#endif
#ifndef __VECTOR__
#include <vector>
#endif

#ifndef BOOST_SHARED_PTR_HPP_INCLUDED
#include <boost/shared_ptr.hpp>
#endif

#include <libsrilm/NgramStats.h>

typedef FloatCount NgramFractionalCount;

class NgramFractionalStats: public NgramCounts<NgramFractionalCount>
{
public:
  NgramFractionalStats(Vocab &vocab, unsigned int order) : 
    NgramCounts<NgramFractionalCount>(vocab, order) {};
  virtual ~NgramFractionalStats() {};
};

class NgramsFractionalIter: public NgramCountsIter<NgramFractionalCount>
{
public:
  NgramsFractionalIter(NgramFractionalStats &ngrams, VocabIndex *keys, unsigned order = 1,
		       int (*sort)(VocabIndex, VocabIndex) = 0) :
    NgramCountsIter<NgramFractionalCount>(ngrams, keys, order, sort) {};
  NgramsFractionalIter(NgramFractionalStats &ngrams, const VocabIndex *start,
		       VocabIndex *keys, unsigned order = 1,
		       int (*sort)(VocabIndex, VocabIndex) = 0) :
    NgramCountsIter<NgramFractionalCount>(ngrams, start, keys, order, sort) {};
};

/**
   Soft counter.
   Based on the article "Discovering Chinese Words from Unsegmented Text".
   The idea is that we count all possible words in a sentence with a fraction count 
   instead of just count the words of the best segmentation in the sentence (count=1)
 */

class SoftCounter
{
public:
	void count(const Lattice &words,NgramFractionalStats &stats);
	void count(const DAG &words,NgramFractionalStats &stats);
	void count(const DAG &words,NgramStats &stats);
};

#endif
