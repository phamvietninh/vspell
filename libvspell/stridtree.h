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

class NNode;
class BranchNNode;
class LeafNNode;
typedef boost::shared_ptr<NNode> NNodeRef;

class NNode
{
public:
  virtual bool is_leaf() const = 0;
  virtual ~NNode() = 0;
};

class BranchNNode:public NNode
{
protected:
  typedef std::multimap<strid,NNodeRef> node_map;
  typedef std::pair<node_map::const_iterator,node_map::const_iterator> const_np_range;
  typedef std::pair<node_map::iterator,node_map::iterator> np_range;
  node_map nodes;
public:
  virtual ~BranchNNode() {}
  bool is_leaf() const { return false; }
  void get_leaves(strid,std::vector<LeafNNode*> &nodes) const;
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
public:
  virtual ~LeafNNode() {}
  bool is_leaf() const { return true; }
  strid get_id() const { return id; }
  void set_id(strid _id) { id = _id; }

  uint get_syllable_count() const;
  void get_syllables(std::vector<strid> &syllables) const;

};

struct DNNode {
  NNode* node;
  int distance;
  DNNode(NNode* _node = NULL):node(_node),distance(0) {}
  int operator == (const DNNode &dn1) const {
    return dn1.node == node;
  }
  int operator < (const DNNode &dn1) const {
    return (int)node+distance < (int)dn1.node+dn1.distance;
  }
  NNode& operator* () const { return *node; }
  NNode* operator-> () const { return node; }
};

