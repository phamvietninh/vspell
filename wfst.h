#ifndef __WFST_H__ // -*- mode: c++ -*-
#define __WFST_H__

#include <vector>
#include <string>

class Comparator
{
public:
};
/*
class Word
{
private:
  std::string::iterator;
  int len;
public:
  bool is_proper_name();
  bool 
};

class Sentence
{
private:
  std::string sentence_;

public:
  class iterator	       
  {
  private:
    std::string::iterator iter_;
  public:
    const_iterator(std::string::iterator iter) {
      iter_ = iter;
    }
    Word& operator++();
    Word operator++(int) {
      Word tmp = *this;
      ++*this;
      return tmp;
    }
  }

  iterator begin() {
    return iterator(sentence_::begin());
  }

  iterator end() {
    return iterator(sentence_::end());
  }
};
*/

class Sentence
{
public:
  class Syllable
  {
  private:
    int start,len;
    Sentence *sent_;
  public:
    int category;
    int span;

    std::string operator* () { return sent_->sent_.substr(start,start+len); }
  };

private:
  std::string sent_;
  vector<Syllable> syllables;
  friend class Syllable;

public:
  Sentence(const std::string &st):sent_(st) {}
  void tokenize();
  void standardize();
  int get_syllable_number() { return syllables.size(); }
  //  void get_word_number() { return word.size(); }
  Syllable& operator[] (int i) { return syllables[i]; }
  //  Syllable& operator[] (int i) { return syllables[i]; }
};

class WordList;

class WordNode
{
private:
  WordList *wl;
public:
  virtual WordNode* get_next(const std::string &str) const;
  float get_prob() const;
};

class WordList:public WordNode
{
public:

  WordNode* get_next(const std::string &str) const;

  bool load(const std::string &filename);
  bool save(const std::string &filename);
};

class WFST
{
protected:
  WordList *wl;
  
public:
  struct Separator
  {
    int anchor;
    float prob;
  };
  struct Segmentation
  {
    std::vector<Separator> sep_list;
    float prob;
  };

  bool set_wordlist(WordList *_wl) {
    wl = _wl;
    return true;
  }

  void segment_best(const std::string &sent,Segmentation &seps);
  template <class OutputIterator>
  void segment_all(const std::string &sent,OutputIterator iter);
};

#endif
