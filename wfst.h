#ifndef __WFST_H__ // -*- mode: c++ -*-
#define __WFST_H__

#include <vector>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

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

//class WordList;
class WordNode;
//typedef boost::shared_ptr<WordNode> WordNodePtr;
typedef WordNode* WordNodePtr;
//typedef boost::shared_ptr<WordList> WordListPtr;

class WordNode
{
private:
  //  WordNodePtr wl;
  typedef std::map<const std::string,WordNodePtr> node_map;
  const std::string syllable;
  node_map nodes;
  float prob;

public:
  WordNode(const std::string &_syllable):prob(-1),syllable(_syllable) {}
  const std::string& get_syllable() const { return syllable; }
  virtual WordNodePtr get_next(const std::string &str) const;
  bool is_next_empty() { return nodes.empty(); }
  virtual void fuzzy_get_next(const std::string &str,std::vector<WordNodePtr>& _nodes) const;
  virtual WordNodePtr create_next(const std::string &str);
  float get_prob() const { return prob; }
  void set_prob(float _prob) { prob = _prob; }

  bool load(const std::string &filename);
  bool save(const std::string &filename);
};

#define SEGM_SEPARATOR 1

struct Segmentation
{
  struct Item {
    int flags;			// Separator mark
    int distance;		// from ed() or fuzzy syllable
    WordNodePtr state;		// used to get prob.

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
  WordNodePtr wl;
  
public:
  bool set_wordlist(WordNodePtr _wl) {
    wl = _wl;
    return true;
  }

  void segment_best(const Sentence &sent,Segmentation &seps);
  template <class OutputIterator>
  void segment_all(const Sentence &sent,OutputIterator iter);
};

#endif
