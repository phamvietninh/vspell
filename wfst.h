#ifndef __WFST_H__ // -*- mode: c++ -*-
#define __WFST_H__

#include <vector>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include "dictionary.h"

#define SEGM_SEPARATOR 1

class Sentence
{
public:
  class Syllable
  {
  private:
  public:
    int start,end;
    //std::string::iterator start,end;
    Sentence *sent_;
    int category;
    int span;

    std::string operator* () const {
      return sent_->sent_.substr(start,end-start); 
    }
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
    Dictionary::WordNodePtr state;		// used to get prob.

    Item():flags(0),distance(0),state(NULL) {}
  };

  std::vector<Item> items;
  float prob;			// total prob
  int distance;			// total distance

  Segmentation():prob(0),distance(0) {}
  void print(const Sentence &st);
};

class WFST
{
protected:
  Dictionary::WordNodePtr wl;
  
public:
  bool set_wordlist(Dictionary::WordNodePtr _wl) {
    wl = _wl;
    return true;
  }

  void segment_best(const Sentence &sent,Segmentation &seps);
  template <class OutputIterator>
  void segment_all(const Sentence &sent,OutputIterator iter);
};

#endif
