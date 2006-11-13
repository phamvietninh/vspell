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
	ifstream ifs(filename);

	if (!ifs.is_open())
		return false;

	int nr_lines = 0;
	string line;
	int start,len,tmp_char;
	string::size_type pos;
	char *str_pos;
	vector<string> toks;
	while (getline(ifs,line)) {
		start = 0;
		len = line.size();
		toks.clear();
		while (start < len) {
			str_pos = strchr(line.c_str()+start,'#');
			if (str_pos == NULL)
				pos = len;
			else
				pos = str_pos - line.c_str();
			tmp_char = line[pos];
			line[pos] = 0;
			toks.push_back(line.c_str()+start);
			line[pos] = tmp_char;
			start = pos+1;
		}

		if (!toks.size())
			continue;			// unrecoverable error
		if (toks.size() < 2)
			toks.push_back("N");	// assume N
		if (toks.size() < 3)
			toks.push_back("0");	// assume 0

		add_entry(toks);
		add_case_entry(toks);

		nr_lines ++;
	}

	return true;
}

LeafNNode* WordArchive::add_special_entry(strid tok)
{
	LeafNNode *leaf = new LeafNNode;
	NNodeRef noderef(leaf);
	vector<strid> toks;
	toks.push_back(tok);
	leaf->set_id(toks);
	//leaf->set_mask(MAIN_LEAF);
	get_root()->add(tok,noderef);
	return leaf;
}

void WordArchive::add_entry(vector<string> toks)
{
	unsigned start,len,pos;
	start = 0;
	len = toks[0].size();
	vector<VocabIndex> syllables;
	while (start < len) {
		pos = toks[0].find(' ',start);
		if (pos == string::npos)
			pos = len;
		string s = toks[0].substr(start,pos-start);
		VocabIndex id = sarch[get_std_syllable(s)];
		syllables.push_back(id);
		start = pos+1;
	}

	vector<strid> path = syllables;
	BranchNNode* branch = get_root()->add_path(path);
	NNodeRef noderef(new LeafNNode);
	LeafNNode *leaf = (LeafNNode*)noderef.get();
	//leaf->set_mask(MAIN_LEAF);
	branch->add(mainleaf_id,noderef);
	leaf->set_id(syllables);

}

void WordArchive::add_case_entry(vector<string> toks2)
{
	vector<string> toks = toks2;
	unsigned start,len,pos;
	start = 0;
	len = toks[0].size();

	transform(toks[0].begin(),toks[0].end(),toks[0].begin(),viet_tolower);

	if (toks[0] == toks2[0])
		return;

	start = 0;										// path
	vector<VocabIndex> syllables;
	while (start < len) {
		pos = toks[0].find(' ',start);
		if (pos == string::npos)
			pos = len;
		string s = toks[0].substr(start,pos-start);
		VocabIndex id = sarch[get_std_syllable(s)];
		syllables.push_back(id);
		start = pos+1;
	}

	start = 0;										// real syllable
	vector<VocabIndex> real_syllables;
	while (start < len) {
		pos = toks2[0].find(' ',start);
		if (pos == string::npos)
			pos = len;
		string s = toks2[0].substr(start,pos-start);
		VocabIndex id = sarch[get_std_syllable(s)];
		real_syllables.push_back(id);
		start = pos+1;
	}

	vector<strid> path = syllables;
	BranchNNode* branch = get_root()->add_path(path);
	NNodeRef noderef(new LeafNNode);
	LeafNNode *leaf = (LeafNNode*)noderef.get();
	//leaf->set_mask(CASE_LEAF);
	branch->add(caseleaf_id,noderef);
	leaf->set_id(real_syllables);
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
	return is;
}
