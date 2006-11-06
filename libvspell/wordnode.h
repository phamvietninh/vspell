#ifndef __WORDNODE_H__		// -*- tab-width: 2 mode: c++ -*-
#define __WORDNODE_H__

#ifndef __TYPES_H__
#include "types.h"
#endif

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

class NNode;
class BranchNNode;
class LeafNNode;
typedef boost::shared_ptr<NNode> NNodeRef;

class NNode
{
public:
  virtual bool is_leaf() const = 0;
  virtual ~NNode() {}
};

class BranchNNode:public NNode
{
public:
  typedef std::map<strid,NNodeRef> node_map;
  typedef std::pair<node_map::const_iterator,node_map::const_iterator> const_np_range;
  typedef std::pair<node_map::iterator,node_map::iterator> np_range;
protected:
  node_map nodes;
public:
  virtual ~BranchNNode() {}
  bool is_leaf() const { return false; }
  LeafNNode* get_leaf(strid leaf) const;
  void get_leaves(std::vector<LeafNNode*> &nodes) const;
  void get_branches(strid,std::vector<BranchNNode*> &nodes) const;
  BranchNNode* get_branch(strid) const;
  const node_map& get_nodes() const { return nodes; }

  BranchNNode* add_path(std::vector<strid> toks);
  void add(strid,NNodeRef);
};

class LeafNNode:public NNode
{
protected:
  strid id;			// word id
	std::vector<strid> syllables;
	uint bitmask;
public:
	LeafNNode():bitmask(0) {}
  virtual ~LeafNNode() {}
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
struct DNNode {
  T* node;
  int distance;
  DNNode(T* _node = NULL):node(_node),distance(0) {}
  int operator == (const DNNode &dn1) const {
    return dn1.node == node;
  }
  int operator < (const DNNode &dn1) const {
    return (int)node+distance < (int)dn1.node+dn1.distance;
  }
  T& operator* () const { return *node; }
  T* operator-> () const { return node; }
};

class WordArchive
{
protected:
	NNodeRef root;
	void add_entry(std::vector<std::string> toks);
	void add_case_entry(std::vector<std::string> toks);
	std::vector<strid> leaf_id;

public:
	WordArchive():root(new BranchNNode) {}
	void init(); // called after LM is initialized
	BranchNNode* get_root() { return (BranchNNode*)root.get(); }
	bool load(const char *filename);
	LeafNNode* add_special_entry(strid);
	void register_leaf(strid leaf);
	const std::vector<strid>& get_leaf_id() const { return leaf_id; }
};
LeafNNode* get_special_node(int);

extern WordArchive warch;

template<class T>
std::ostream& operator << (std::ostream &os,const DNNode<T> &node)
{
	os << *node.node;
	return os;
}

std::ostream& operator << (std::ostream &os,const LeafNNode &node);

#endif
