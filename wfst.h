#ifndef __WFST_H__ // -*- mode: c++ -*-
#define __WFST_H__

#include <vector>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
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

    Dictionary::strid get_id() const { return id; }
    Dictionary::strid get_cid() const { return cid; }
  };

private:
  std::string sent_;
  vector<Syllable> syllables;
  friend class Syllable;

public:
  Sentence(const std::string &st):sent_(st) {}
  void set_sentence(const std::string &st) { sent_ = st; syllables.clear(); }
  void tokenize();
  void standardize();
  int get_syllable_count() const { return syllables.size(); }
  //  void get_word_number() { return word.size(); }
  Syllable& operator[] (int i) { return syllables[i]; }
  Syllable operator[] (int i) const { return syllables[i]; }
  //  Syllable& operator[] (int i) { return syllables[i]; }
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

class WFST
{
protected:
  Dictionary::WordNodePtr wl;

  struct WordInfo {
    Dictionary::WordNode::DistanceNode exact_match;
    vector<Dictionary::WordNode::DistanceNode> fuzzy_match;
  };
public:
  typedef vector<WordInfo> WordInfos;

  bool set_wordlist(Dictionary::WordNodePtr _wl) {
    wl = _wl;
    return true;
  }

  void get_all_words(const Sentence &sent,
		     vector<WordInfos> &words);
  void segment_best(const Sentence &sent,
		    const vector<WordInfos> &words,
		    Segmentation &seps);
  void segment_all(const Sentence &sent,
		   std::vector<Segmentation> &result);
private:
  void segment_all1(const Sentence &sent,
		    const vector<WordInfos> &words,
		    int from,int to,
		    std::vector<Segmentation> &result);
  
};

#endif
