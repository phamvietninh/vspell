#ifndef __WFST_H__ // -*- tab-width: 2 mode: c++ -*-
#define __WFST_H__

#include <vector>
#include <string>
#include <iostream>
#include "dictionary.h"

#define SEGM_SEPARATOR 1

class Sentence
{
public:
	class Syllable
	{
	private:
	public:
		int start;
		Dictionary::strid id,cid;
		//std::string::iterator start,end;
		Sentence *sent_;
		int category;
		int span;
		int sid;

		Dictionary::strid get_id() const { return sid >= 0 ? sid : id; }
		Dictionary::strid get_cid() const { return sid >= 0 ? sid : cid; }
	};

private:
	std::string sent_;
	std::vector<Syllable> syllables;
	friend class Syllable;

	void tokenize_punctuation(const std::string &s,std::vector<std::string> &ss);

public:
	Sentence(const std::string &st):sent_(st) {}
	void set_sentence(const std::string &st) { sent_ = st; syllables.clear(); }
	std::string get_sentence() { return sent_; }
	void tokenize();
	void standardize();
	int get_syllable_count() const { return syllables.size(); }
	//  void get_word_number() { return word.size(); }
	Syllable& operator[] (int i) { return syllables[i]; }
	Syllable operator[] (int i) const { return syllables[i]; }
	//  Syllable& operator[] (int i) { return syllables[i]; }
	bool is_contiguous(int i);		// i & i+1 is contiguous ?
	void merge(int i);
};

struct Segmentation
{
	struct Item {
		int flags;			// Separator mark
		int distance;		// from ed() or fuzzy syllable
		Dictionary::WordNodePtr state; // used to get prob.

		Item():flags(0),distance(0),state(NULL) {}
	};

	std::vector<Item> items;
	float prob;			// total prob
	int distance;			// total distance

	Segmentation():prob(0),distance(0) {}
	void print(std::ostream &os,const Sentence &st);
};

struct WordInfo {
	Dictionary::WordNode::DistanceNode exact_match;
	std::vector<Dictionary::WordNode::DistanceNode> fuzzy_match;
};
typedef std::vector<WordInfo*> WordInfos;
class Words:public std::vector<WordInfos*> {
public:
	int get_word_count() const { return size(); }
	int get_len(int i) const { return (*this)[i]->size(); }
	int get_fuzzy_count(int i,int l) const { 
		return (*(*this)[i])[l]->fuzzy_match.size();
	}
	Dictionary::WordNode::DistanceNode& get_fuzzy(int i,int l,int f) {
		return (*(*this)[i])[l]->fuzzy_match[f];
	}
	const Dictionary::WordNode::DistanceNode& get_fuzzy(int i,int l,int f) const{
		return (*(*this)[i])[l]->fuzzy_match[f];
	}
	~Words();											// WARN: destroy all.
};
// Words[from][len].fuzzy_match[i]

class WFST
{
protected:
	Dictionary::WordNodePtr wl;  
	bool ngram_enabled;

public:
	WFST():wl(NULL),ngram_enabled(false) {}

	bool set_wordlist(Dictionary::WordNodePtr _wl) {
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
