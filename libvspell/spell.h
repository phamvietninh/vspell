#ifndef __SPELL_H__ // -*- tab-width: 2 mode: c++ -*-
#define __SPELL_H__

#ifndef __DICTIONARY_H__
#include "dictionary.h"
#endif

#ifndef __VECTOR__
#include <vector>
#endif
#ifndef __STRING__
#include <string>
#endif

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

struct Segmentation
{
  struct Item {
    int flags;			// Separator mark
    int distance;		// from ed() or fuzzy syllable
    WordNodePtr state; // used to get prob.

    Item():flags(0),distance(0),state(NULL) {}
  };

  std::vector<Item> items;
  float prob;			// total prob
  int distance;			// total distance

  Segmentation():prob(0),distance(0) {}
  void print(std::ostream &os,const Sentence &st);
};

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
