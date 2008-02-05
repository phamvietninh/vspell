#include "wordnode.h"		// -*- tab-width: 2 coding: viscii mode: c++ -*-
#include "syllable.h"
#include <utility>
#include <fstream>
#include <iostream>
#include <boost/format.hpp>

using namespace std;
static strid mainleaf_id,caseleaf_id;
std::map<strid,LeafNNode*> LeafNNode::leaf_index;

LeafNNode* BranchNNode::get_leaf(strid leaf) const
{
	node_map::const_iterator iter;
	iter = nodes.find(leaf);
	if (iter != nodes.end())
		return((LeafNNode*)iter->second.get());
	else
		return NULL;
}

void BranchNNode::get_leaves(std::vector<LeafNNode*> &_nodes) const
{
	const vector<strid> leaf_id = warch.get_leaf_id();
	node_map::const_iterator iter;
	uint i,n = leaf_id.size();
	for (i = 0;i < n;i ++) {
		iter = nodes.find(leaf_id[i]);
		if (iter != nodes.end())
			_nodes.push_back((LeafNNode*)iter->second.get());
	}
}

void BranchNNode::get_branches(strid _id,std::vector<BranchNNode*> &_nodes) const
{
  const_np_range range;
	range = nodes.equal_range(_id);
	node_map::const_iterator iter;
	for (iter = range.first;iter != range.second; ++iter)
		if (!iter->second->is_leaf())
			_nodes.push_back((BranchNNode*)iter->second.get());
}

BranchNNode* BranchNNode::get_branch(strid _id) const
{
	const_np_range range;
	range = nodes.equal_range(_id);
	node_map::const_iterator iter;
	for (iter = range.first;iter != range.second; ++iter)
		if (!iter->second->is_leaf())
			return (BranchNNode*)iter->second.get();
	return NULL;
}

void BranchNNode::add(strid _id,NNodeRef _branch)
{
	nodes.insert(make_pair(_id,_branch));
}

BranchNNode* BranchNNode::add_path(const std::vector<strid> &toks)
{
	uint i,n = toks.size();
	BranchNNode *me = this;
	for (i = 0;i < n;i ++) {
		BranchNNode *next = me->get_branch(toks[i]);
		if (next == NULL) {
			NNodeRef branch(new BranchNNode());
			me->add(toks[i],branch);
			next = (BranchNNode*)branch.get();
		}
		me = next;
	}
	return me;
}

void WordArchive::init()
{
	mainleaf_id = sarch["<mainleaf>"];
	caseleaf_id = sarch["<caseleaf>"];
	register_leaf(mainleaf_id);
	register_leaf(caseleaf_id);
}

bool WordArchive::load(const char* filename)
{
	if (filename != NULL) {
		ifstream ifs(filename);

		if (!ifs.is_open())
			return false;

		string word;
		while (ifs >> word) {
			add_entry(word.c_str());
			add_case_entry(word.c_str());
		}
	}
	else {
		const lm_t * lm = get_ngram().get_lm();
		for (int i = 0;i < lm->ucount;i ++) {
			add_entry(lm->word_str[i]);
			add_case_entry(lm->word_str[i]);
		}
	}
	return true;
}

LeafNNode* WordArchive::add_special_entry(strid tok)
{
	return add_entry(sarch[tok]);
	/*
	LeafNNode *leaf = new LeafNNode;
	NNodeRef noderef(leaf);
	vector<strid> toks;
	toks.push_back(tok);
	leaf->set_id(toks);
	//leaf->set_mask(MAIN_LEAF);
	get_root()->add(tok,noderef);
	return leaf;
	*/
}

LeafNNode* WordArchive::add_entry(const char *w)
{
	unsigned len,wlen;
	const char *pos,*start;
	char *buf;
	len = strlen(w);
	start = pos = w;
	buf = (char *)malloc(len+1);
	vector<VocabIndex> syllables;
	while (pos) {
		pos = strchr(start,'_');
		wlen = pos ? pos - start : len - (start - w);
		memcpy(buf,start,wlen);
		buf[wlen] = '\0';
		VocabIndex id = sarch[buf];
		syllables.push_back(id);
		start = pos+1;
	}
	free(buf);

	vector<strid> path = syllables;
	BranchNNode* branch = get_root()->add_path(path);
	assert (!branch->get_leaf(mainleaf_id));
	NNodeRef noderef(new LeafNNode);
	LeafNNode *leaf = (LeafNNode*)noderef.get();
	//leaf->set_mask(MAIN_LEAF);
	branch->add(mainleaf_id,noderef);
	leaf->set_id(syllables);
	return leaf;
}

LeafNNode* WordArchive::add_case_entry(const char *w2)
{
	unsigned i,same,len,wlen;
	const char *pos,*start;
	char *buf;
	char *w;
	len = strlen(w2);
	w = (char *)malloc(len+1);
	same = 1;
	for (i = 0;i < len;i ++) {
		w[i] = (char)viet_tolower(w2[i]);
		if (same && w[i] != w2[i])
			same = 0;
	}
	if (same) {
		free(w);
		return NULL;
	}
	w[len] = '\0';
	buf = (char *)malloc(len+1);
	vector<VocabIndex> syllables,real_syllables;
	start = pos = w;
	while (pos) {
		pos = strchr(start,'_');
		wlen = pos ? pos - start : len - (start - w);
		memcpy(buf,start,wlen);
		buf[wlen] = '\0';
		VocabIndex id = sarch[buf];
		syllables.push_back(id);
		start = pos+1;
	}
	free(w);

	start = pos = w2;
	while (pos) {
		pos = strchr(start,'_');
		wlen = pos ? pos - start : len - (start - w2);
		memcpy(buf,start,wlen);
		buf[wlen] = '\0';
		VocabIndex id = sarch[buf];
		real_syllables.push_back(id);
		start = pos+1;
	}
	free(buf);

	vector<strid> path = syllables;
	BranchNNode* branch = get_root()->add_path(path);
	assert(!branch->get_leaf(caseleaf_id));
	NNodeRef noderef(new LeafNNode);
	LeafNNode *leaf = (LeafNNode*)noderef.get();
	//leaf->set_mask(CASE_LEAF);
	branch->add(caseleaf_id,noderef);
	leaf->set_id(real_syllables);
	return leaf;
}

void WordArchive::register_leaf(strid id)
{
	if (find(leaf_id.begin(),leaf_id.end(),id) == leaf_id.end())
		leaf_id.push_back(id);
}

void LeafNNode::set_mask(uint maskval,bool mask)
{
	if (mask)
		bitmask |= maskval;
	else
		bitmask &= ~maskval;
}

void LeafNNode::set_id(const vector<strid> &_syllables)
{
	syllables = _syllables;
	string word;
	int i,nr_syllables = syllables.size();
	for (i = 0;i < nr_syllables;i ++) {
		if (i)
			word += "_";
		word += sarch[syllables[i]];
	}
	id = sarch[word];
	leaf_index[id] = this;
}

LeafNNode* LeafNNode::find_leaf(const vector<strid> &syllables)
{
	string word;
	int i,nr_syllables = syllables.size();
	for (i = 0;i < nr_syllables;i ++) {
		if (i)
			word += "_";
		word += sarch[syllables[i]];
	}
	strid id = sarch[word];
	map<strid,LeafNNode*>::iterator iter = leaf_index.find(id);
	return iter != leaf_index.end() ? iter->second : NULL;
}

std::ostream& operator << (std::ostream &os,const LeafNNode &node)
{
	std::vector<strid> syll;
	node.get_syllables(syll);
	os << boost::format("%04x %d") % node.bitmask % syll.size();
	for (std::vector<strid>::size_type i = 0;i < syll.size();i ++) {
		os << " ";
		os << sarch[syll[i]];
	}
	return os;
}

std::istream& operator >> (std::istream &is,LeafNNode* &node)
{
	std::vector<strid> syll;
	int n;
	uint bitmask;
	is >> hex >> bitmask >> dec >> n;
	syll.resize(n);
	for (std::vector<strid>::size_type i = 0;i < syll.size();i ++) {
		string s;
		is >> s;
		syll[i] = get_ngram()[s];
	}
	node = LeafNNode::find_leaf(syll);
	assert(node);
	return is;
}
