#ifndef __SPELL_H__ // -*- tab-width: 2 mode: c++ -*-
#define __SPELL_H__

#ifndef __DICTIONARY_H__
#include "dictionary.h"
#endif

#ifndef __WORDNODE_H__
#include "wordnode.h"
#endif

#ifndef __VECTOR__
#include <vector>
#endif
#ifndef __STRING__
#include <string>
#endif

#ifndef BOOST_SHARED_PTR_HPP_INCLUDED
#include <boost/shared_ptr.hpp>
#endif

class Sentence;

/**
	 Store information of a word in a sentence.
 */

struct WordEntry {
	int pos;											/// syllable index
	int len;											/// word len
	int fuzid; /// to identify different WordEntry with the same pos/len.
																/// fuzid is a mask of fuzzy/exact match.
	int id;												/// index in WordEntries

	WordNode::DistanceNode node;	/// Word content

	bool operator < (const WordEntry &we) const {
		return pos != we.pos ? pos < we.pos :
			(len != we.len ? len < we.len : 
			 (fuzid != we.fuzid ? fuzid < we.fuzid : node.node < we.node.node));
	}
};

std::ostream& operator << (std::ostream &os,const WordEntry &we);

typedef WordEntry* WordEntryRef;
typedef std::vector<WordEntry> WordEntries;
typedef std::vector<WordEntryRef> WordEntryRefs;
//struct WordEntries:public std::vector<WordEntry> {
//};

/**
	 Store all WordEntry pointers which started at a specified pos, 
	 and have specified len
 */

struct WordInfo {
	WordEntry* exact_match;				
	WordEntryRefs fuzzy_match;	 
};

/*
class MonitoredWordEntryRefs:public WordEntryRefs {
public:
	void push_back(const WordEntryRef &w) {
		if (&w == 0) 0/0;
		WordEntryRefs::push_back(w);
	}
};
*/
/**
	 Store WordInfo(s) which have a specified length
 */

class WordInfos : public std::vector<WordInfo*> {
public:
	WordInfos();
	int exact_len;
	WordEntryRefs fuzzy_map; /// contains all WordEntry which are fuzzy at this position
	WordEntryRefs we;	/// contains all WordEntry which started at this pos
};

/**
	 Store WordInfos(s) started at specified positions.
 */

class Words:public std::vector<WordInfos*> {
public:

	boost::shared_ptr<WordEntries> we;
	Sentence *st;

	void construct(const Sentence &st);

	/// Get the number of available positions, from 0 to n-1
	int get_word_count() const { 
		return size(); 
	}

	/**
		 Get maximal length of words at specified position.
		 \param i specify a position in sentence
	*/
	int get_len(int i) const {
		return (*this)[i]->size(); 
	}

	/**
		 Get the length of the exact words at specified position.
		 \param i specify a position in sentence
	*/
	int get_exact_len(int i) const {
		return (*this)[i]->exact_len; 
	}

	/**
		 Get fuzzy map at specified position.
		 \param i specify a position
	 */

	const WordEntryRefs& get_fuzzy_map(int i) const {
		return (*this)[i]->fuzzy_map; 
	}

	/**
		 Get number of fuzzy words at specified (pos,len).
		 \param i specify pos
		 \param l specify len
	 */
	int get_fuzzy_count(int i,int l) const { 
		return (*(*this)[i])[l]->fuzzy_match.size();
	}

	/**
		 Get the first WordEntry at specified pos.
		 \param pos specify a position.
	 */

	WordEntryRefs& get_we(int pos) {
		return (*this)[pos]->we;
	}

	const WordEntry& get_we_fuzzy(int i,int l,int f) const {
		return *(*(*this)[i])[l]->fuzzy_match[f];
	}

	const WordEntry* get_we_exact(int i,int l) const {
		return (*(*this)[i])[l]->exact_match;
	}

	/**
		 Get the Node of a specified fuzzy match  (pos,len,fuz)
		 \param i is pos
		 \param l is len
		 \param f is fuz
	 */

	WordNode::DistanceNode& get_fuzzy(int i,int l,int f) {
		return (*(*this)[i])[l]->fuzzy_match[f]->node;
	}

	/// a const version of get_fuzzy()
	const WordNode::DistanceNode& get_fuzzy(int i,int l,int f) const{
		return (*(*this)[i])[l]->fuzzy_match[f]->node;
	}

	~Words();											// WARN: destroy all.

	/**
		 Construct Words based on member we.
		 we must be valid.
	 */

	void construct();	

	/**
		 Construct Words based on another Words.
		 Only exact matches are copied.
		 \param w specify the "template" Words
	 */

	void based_on(const Words &w);

	/**
		 Add WordEntry w into Words.
	 */

	void add(WordEntry &w);
  
	friend std::ostream& operator << (std::ostream& os,const Words &w);
}; 
// Words[from][len].fuzzy_match[i]

/**
	 Sentence is used to store a sequence of syllables.
	 Sentence and Words will keep all necessary info for a spelling checker.
	 "Sentence" here is not exactly a sentence. It's just a part of sentence
	 separated by punctuation.
 */

class Sentence
{
public:
  class Syllable
  {
  private:
  public:
    int start;
    strid id,cid;
    //std::string::iterator start,end;
    Sentence *sent_;
    int category;
    int span;
    int sid;										// faked id

    strid get_id() const { return sid >= 0 ? sid : id; }
    strid get_cid() const { return sid >= 0 ? sid : cid; }
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

typedef Sentence* SentenceRef;

/**
	 Segmentation store a sequence of WordEntry index.
	 From WordEntry we can get the real word.
 */

struct Segmentation : public std::vector<int> 
{
	boost::shared_ptr<WordEntries> we; /// WordEntries associated with Segmentation
  float prob;										/// total prob
  int distance;									/// total distance

  Segmentation():prob(0),distance(0) {}
	const WordEntry& operator[] (int id) const {
		return (*we)[std::vector<int>::operator[](id)];
	}
	friend std::ostream& operator <<(std::ostream &os,const Segmentation &seg);
};

typedef std::vector<Segmentation> Segmentations;

struct Suggestion {
  int id;
  std::vector<strid> suggestions;
};

typedef std::vector<Suggestion> Suggestions;

//namespace Spell {
void spell_check1(Sentence &st,Suggestions &s);
void spell_check2(Sentence &st,Segmentation &seg,Suggestions &s);
//}


#endif
