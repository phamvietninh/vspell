#ifndef __WORDNODE_H__		// -*- tab-width: 2 mode: c++ -*-
#define __WORDNODE_H__

#ifndef __VECTOR__
#include <vector>
#endif

#ifndef __DICTIONARY_H__
#include "dictionary.h"
#endif

#ifndef __SYLLABLE_H__
#include "syllable.h"
#endif

class WordNode;
typedef WordNode* WordNodePtr;

class WordNode {
protected:
  struct WordInfo {
    VocabIndex id;
    float prob;
    VocabIndex* syllables;
    int a,b;			// a/b
    WordInfo():prob(0),syllables(NULL),a(0),b(0) {}
    //      ~WordInfo() { if (syllables) delete[] syllables; }
  };
  //  WordNodePtr wl;
  typedef SArray<strid,WordNodePtr> node_map;
  typedef SArrayIter<strid,WordNodePtr> node_map_iterator;
  node_map *nodes;
  WordInfo *info;
	VocabIndex id;

public:
  struct DistanceNode {
    WordNodePtr node;
    int distance;
    DistanceNode(WordNodePtr _node = NULL):node(_node),distance(0) {}
    int operator == (const DistanceNode &dn1) const {
      return dn1.node == node;
    }
    int operator < (const DistanceNode &dn1) const {
      return (int)node+distance < (int)dn1.node+dn1.distance;
    }
		WordNode& operator* () const { return *node; }
		WordNode* operator-> () const { return node; }
  };

  void recalculate();

public:

  WordNode(strid _syllable):info(NULL),nodes(new node_map),id(_syllable) {}
  //    ~WordNode();

  VocabIndex get_syllable() const { return id; }
  virtual WordNodePtr get_next(strid str) const;
  void inc_a() { ASSERT(info != NULL); info->a++; }
  void inc_b() { ASSERT(info != NULL); info->b++; }
  int& get_a() { ASSERT(info != NULL); return info->a; }
  int& get_b() { ASSERT(info != NULL); return info->b; }
  VocabIndex get_id() { return info->id; } // ASSERT: info != NULL
  bool is_next_empty() { return nodes->empty(); }
  virtual bool fuzzy_get_next(strid str,std::vector<DistanceNode>& _nodes) const;
  virtual WordNodePtr create_next(strid str);
  float get_prob() const { return info ? info->prob : -1; }
  void set_prob(float _prob);

  int get_syllable_count() const;
  void get_syllables(std::vector<strid> &syllables) const;
  WordNodePtr follow_syllables(const std::vector<strid> &syllables);

	void dump_next(std::ostream &os) const;

  bool load(const char* filename);
  bool save(const char* filename);
};

class FuzzyWordNode:public WordNode {
protected:
  bool fuzzy_get_next_with_syllable(strid str,
				    std::vector<DistanceNode>& _nodes,
				    const Syllable &syll) const;
  bool fuzzy_get_next_with_ed(strid str,
			      std::vector<DistanceNode>& _nodes,
			      const char *str_data,
			      bool parsable) const;
  bool fuzzy;

public:
  FuzzyWordNode(strid _id):WordNode(_id),fuzzy(true) {}
  virtual bool fuzzy_get_next(strid str,std::vector<DistanceNode>& _nodes) const;
  virtual WordNodePtr create_next(strid str);
  void enable_fuzzy(bool _fuzzy) { fuzzy = _fuzzy; }
};

WordNodePtr get_root();
std::ostream& operator << (std::ostream &os,const WordNode::DistanceNode &dnode);
std::ostream& operator << (std::ostream &os,const WordNode &node);

#endif
