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

struct WordInfo {
	WordNode::DistanceNode exact_match;
	std::vector<WordNode::DistanceNode> fuzzy_match;
};

typedef std::vector<WordInfo*> WordInfos;

class Words:public std::vector<WordInfos*> {
public:
	int get_word_count() const { return size(); }
	int get_len(int i) const { return (*this)[i]->size(); }
	int get_fuzzy_count(int i,int l) const { 
		return (*(*this)[i])[l]->fuzzy_match.size();
	}
	WordNode::DistanceNode& get_fuzzy(int i,int l,int f) {
		return (*(*this)[i])[l]->fuzzy_match[f];
	}
	const WordNode::DistanceNode& get_fuzzy(int i,int l,int f) const{
		return (*(*this)[i])[l]->fuzzy_match[f];
	}
	~Words();											// WARN: destroy all.
	void print() const;
};
// Words[from][len].fuzzy_match[i]

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

	void get_all_words(const Sentence &sent,
				 Words &words);
	void segment_best(const Sentence &sent,
				const Words &words,
				Segmentation &seps);
	void segment_all(const Sentence &sent,
			 std::vector<Segmentation> &result);
private:
	void segment_all1(const Sentence &sent,
				const Words &words,
				int from,int to,
				std::vector<Segmentation> &result);
	
};

#endif
