#ifndef __WORDNODE_H__		// -*- tab-width: 2 mode: c++ -*-
#define __WORDNODE_H__

#ifndef __DICTIONARY_H__
#include "dictionary.h"
#endif
#ifndef __VECTOR__
#include <vector>
#endif

#ifndef __MAP__
#include <map>
#endif

#ifndef BOOST_SHARED_PTR_HPP_INCLUDED
#include <boost/shared_ptr.hpp>
#endif

#ifndef __MYSTRING_H__
#include "mystring.h"
#endif

#define ALL_LEAVES 0						// all leaves
#define MAIN_LEAF 1							// normal leaves
#define CASE_LEAF 2							// lower-to-realcase leaves

class Node;
class BranchNode;
class LeafNode;
typedef boost::shared_ptr<Node> NodeRef;

class Node
{
public:
  virtual bool is_leaf() const = 0;
  virtual ~Node() {}
};

class BranchNode:public Node
{
public:
  typedef std::multimap<strid,NodeRef> node_map;
  typedef std::pair<node_map::const_iterator,node_map::const_iterator> const_np_range;
  typedef std::pair<node_map::iterator,node_map::iterator> np_range;
protected:
  node_map nodes;
public:
  virtual ~BranchNode() {}
  bool is_leaf() const { return false; }
  void get_leaves(std::vector<LeafNode*> &nodes,uint mask = MAIN_LEAF) const;
  void get_branches(strid,std::vector<BranchNode*> &nodes) const;
  BranchNode* get_branch(strid) const;
  const node_map& get_nodes() const { return nodes; }

  BranchNode* add_path(std::vector<strid> toks);
  void add(strid,NodeRef);
};

class LeafNode:public Node
{
protected:
  strid id;			// word id
	std::vector<strid> syllables;
	uint bitmask;
public:
	LeafNode():bitmask(0) {}
  virtual ~LeafNode() {}
  bool is_leaf() const { return true; }
  strid get_id() const { return id; }
  void set_id(const std::vector<strid> &sy);

	uint get_mask() const { return bitmask; }
	void set_mask(uint maskval,bool mask = true);
	bool filter(uint mask) const { return (bitmask & mask) == mask; }

  uint get_syllable_count() const { return syllables.size(); }
  void get_syllables(std::vector<strid> &_syllables) const { _syllables = syllables; }

};

template<class T>
struct DNode {
  T* node;
  int distance;
  DNode(T* _node = NULL):node(_node),distance(0) {}
  int operator == (const DNode &dn1) const {
    return dn1.node == node;
  }
  int operator < (const DNode &dn1) const {
    return (int)node+distance < (int)dn1.node+dn1.distance;
  }
  T& operator* () const { return *node; }
  T* operator-> () const { return node; }
};

class WordArchive
{
protected:
	NodeRef root;
	void add_entry(std::vector<std::string> toks);
	void add_case_entry(std::vector<std::string> toks);

public:
	WordArchive();
	BranchNode* get_root() { return (BranchNode*)root.get(); }
	bool load(const char *filename);
	LeafNode* add_special_entry(strid);
};
LeafNode* get_special_node(int);

extern WordArchive warch;

template<class T>
std::ostream& operator << (std::ostream &os,const DNode<T> &node)
{
	os << *node.node;
	return os;
}

std::ostream& operator << (std::ostream &os,const LeafNode &node);

#endif

