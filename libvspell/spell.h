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
#ifndef __SET__
#include <set>
#endif
#ifndef __STRING__
#include <string>
#endif

#ifndef BOOST_SHARED_PTR_HPP_INCLUDED
#include <boost/shared_ptr.hpp>
#endif

typedef unsigned int uint;

class Sentence;

/**
	 Store information of a word in a sentence.
 */

struct WordEntry {
	unsigned int pos;							/// syllable index
	unsigned int len;							/// word len
	unsigned int fuzid;	/// to identify different WordEntry with the same pos/len.
																/// fuzid is a mask of fuzzy/exact match.
	unsigned int id;							/// index in WordEntries

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


/**
	 Store WordInfo(s) which have a specified length
 */

class WordInfos {
public:
	WordInfos();
	unsigned int exact_len;
	WordEntryRefs fuzzy_map; /// contains all WordEntry which are fuzzy at this position
	WordEntryRefs we;	/// contains all WordEntry which started at this pos
};

class WordState;
typedef std::vector<WordState*> WordStates;

/**
	Store information used by WFST::get_all_words().
	This is a self-destroy object. get_next(), get_first() will destroy itself if necessary.
*/
class WordState {
protected:
	WordState(const WordState&);

public:
	/**
		 the currently processing node
	 */
	WordNode::DistanceNode dnode;
	const Sentence &sent;
	//bool survive;

	int fuzid;
	int pos;
	WordState(const Sentence &st):fuzid(0),sent(st),pos(0) {}
	virtual void get_first(WordStates &states,uint pos);
	virtual void get_next(WordStates &states,uint pos) = 0;	// you have to delete your self after this if your task is done
};


struct WordStateFactory {
	virtual void create_new(WordStates &states,uint pos,const Sentence &st) const = 0;
};

typedef std::vector<WordStateFactory*> WordStateFactories;

#define WORDSTATEFACTORY(CLASS)\
struct CLASS##Factory:public WordStateFactory { \
	virtual void create_new(WordStates &states,uint pos,const Sentence &st) const {\
		(new CLASS(st))->get_first(states,pos);\
	}\
};

/**
	 Store WordInfos(s) started at specified positions.
 */

class Lattice {
protected:
	std::vector<WordInfos> wi;

public:

	boost::shared_ptr<WordEntries> we;
	Sentence const * st;

	void construct(const Sentence &st);
	void pre_construct(const Sentence &st,std::set<WordEntry> &wes,const WordStateFactories &f);
	void post_construct(std::set<WordEntry> &wes);

	/// Get the number of available positions, from 0 to n-1
	unsigned int get_word_count() const { 
		return wi.size(); 
	}

	/**
		 Get maximal length of words at specified position.
		 \param i specify a position in sentence
	*/
	unsigned int get_len(unsigned int i) const;

	/**
		 Get the length of the exact words at specified position.
		 \param i specify a position in sentence
	*/
	unsigned int get_exact_len(unsigned int i) const {
		return wi[i].exact_len; 
	}

	/**
		 Get fuzzy map at specified position.
		 \param i specify a position
	 */

	const WordEntryRefs& get_fuzzy_map(unsigned int i) const {
		return wi[i].fuzzy_map; 
	}

	/**
		 Get the all WordEntry(s) at specified pos.
		 \param pos specify a position.
	 */

	const WordEntryRefs& get_we(unsigned int pos) const {
		return wi[pos].we;
	}

	~Lattice();											// WARN: destroy all.

	/**
		 Construct Lattice based on member we.
		 we must be valid.
	 */

	void construct();	

	/**
		 Construct Lattice based on another Lattice.
		 Only exact matches are copied.
		 \param w specify the "template" Lattice
	 */

	void based_on(const Lattice &w);

	/**
		 Add WordEntry w into Lattice.
	 */

	void add(WordEntry &w);
  
	friend std::ostream& operator << (std::ostream& os,const Lattice &w);
}; 

struct ExactWordState:public WordState {
	ExactWordState(const Sentence &st):WordState(st) {}
	void get_next(WordStates &states,uint pos);
};
WORDSTATEFACTORY(ExactWordState);

struct LowerWordState:public WordState {
	LowerWordState(const Sentence &st):WordState(st) {}
	void get_next(WordStates &states,uint pos);
};
WORDSTATEFACTORY(LowerWordState);

struct FuzzyWordState:public WordState {
	FuzzyWordState(const Sentence &st):WordState(st) {}
	void get_next(WordStates &states,uint pos);
};
WORDSTATEFACTORY(FuzzyWordState);

struct CaseWordState:public WordState {
	CaseWordState(const Sentence &st):WordState(st) {}
	void get_next(WordStates &states,uint pos);
};
WORDSTATEFACTORY(CaseWordState);


/**
	 Sentence is used to store a sequence of syllables.
	 Sentence and Lattice will keep all necessary info for a spelling checker.
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
    unsigned int start;
    strid id;										/// real string
		strid cid;									/// lowercased string
		//strid iid,icid;
    //std::string::iterator start,end;
    Sentence *sent_;
    unsigned int category;
    unsigned int span;

    strid get_id() const { return id; }
    strid get_cid() const { return cid; }
  };

private:
  std::string sent_;
  std::vector<Syllable> syllables;
  friend class Syllable;

  void tokenize_punctuation(const std::string &s,std::vector<std::string> &ss);

public:
	Sentence() {}
  Sentence(const std::string &st):sent_(st) {}
  void set(const std::string &st) { sent_ = st; syllables.clear(); }
  std::string get() { return sent_; }
  void tokenize();
  void standardize();
  unsigned int get_syllable_count() const { return syllables.size(); }
  //  void get_word_number() { return word.size(); }
  Syllable& operator[] (unsigned int i) { return syllables[i]; }
  Syllable operator[] (unsigned int i) const { return syllables[i]; }
  //  Syllable& operator[] (int i) { return syllables[i]; }
  bool is_contiguous(unsigned int i);		// i & i+1 is contiguous ?
  void merge(unsigned int i);
	friend std::ostream& operator <<(std::ostream &os, const Sentence &st);
};

typedef Sentence* SentenceRef;

/**
	 Segmentation store a sequence of WordEntry index.
	 From WordEntry we can get the real word.
 */

struct Segmentation : public std::vector<uint> 
{
	boost::shared_ptr<WordEntries> we; /// WordEntries associated with Segmentation
  float prob;										/// total prob
  int distance;									/// total distance

  Segmentation(boost::shared_ptr<WordEntries> _we = boost::shared_ptr<WordEntries>()):
		we(_we),
		prob(0),
		distance(0)	{}

	Segmentation(const Segmentation&seg):std::vector<uint>(seg) {
		prob = seg.prob;
		distance = seg.distance;
		we = seg.we;
	}

	const WordEntry& operator[] (int id) const {
		return (*we)[std::vector<uint>::operator[](id)];
	}
	friend std::ostream& operator <<(std::ostream &os,const Segmentation &seg);
	std::ostream& pretty_print(std::ostream &os,const Sentence &st);

};

typedef std::vector<Segmentation> Segmentations;

struct Suggestion {
  int id;
  std::vector<strid> suggestions;
};

typedef std::vector<Suggestion> Suggestions;

void apply_separator(std::set<WordEntry> &wes,int p);

//namespace Spell {
//void spell_check1(Sentence &st,Suggestions &s);
//void spell_check2(Sentence &st,Segmentation &seg,Suggestions &s);
//}


#endif
