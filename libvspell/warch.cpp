#include "wordnode.h"		// -*- tab-width: 2 coding: viscii mode: c++ -*-
#include "syllable.h"
#include <utility>

using namespace std;


void BranchNode::get_leaves(std::vector<LeafNode*> &_nodes,uint mask) const
{
	const_np_range range;
	range = nodes.equal_range(sarch["<leaf>"]);
	node_map::const_iterator iter;
	for (iter = range.first;iter != range.second; ++iter)
		if (iter->second->is_leaf() && ((LeafNode*)&*iter->second)->filter(mask))
			_nodes.push_back((LeafNode*)iter->second.get());
}

void BranchNode::get_branches(strid _id,std::vector<BranchNode*> &_nodes) const
{
  const_np_range range;
	range = nodes.equal_range(_id);
	node_map::const_iterator iter;
	for (iter = range.first;iter != range.second; ++iter)
		if (!iter->second->is_leaf())
			_nodes.push_back((BranchNode*)iter->second.get());
}

BranchNode* BranchNode::get_branch(strid _id) const
{
	const_np_range range;
	range = nodes.equal_range(_id);
	node_map::const_iterator iter;
	for (iter = range.first;iter != range.second; ++iter)
		if (!iter->second->is_leaf())
			return (BranchNode*)iter->second.get();
	return NULL;
}

void BranchNode::add(strid _id,NodeRef _branch)
{
	nodes.insert(make_pair(_id,_branch));
}

BranchNode* BranchNode::add_path(std::vector<strid> toks)
{
	uint i,n = toks.size();
	BranchNode *me = this;
	for (i = 0;i < n;i ++) {
		BranchNode *next = me->get_branch(toks[i]);
		if (next == NULL) {
			NodeRef branch(new BranchNode());
			me->add(toks[i],branch);
			next = (BranchNode*)branch.get();
		}
		me = next;
	}
	return me;
}

WordArchive::WordArchive():root(new BranchNode)
{
}

bool WordArchive::load(const char* filename)
{
	File ifs(filename,"rt");

	if (ifs.error())
		return false;

	int nr_lines = 0;
	char *line;
	int start,len,tmp_char;
	string::size_type pos;
	char *str_pos;
	vector<string> toks;
	while ((line = ifs.getline())) {
		start = 0;
		len = strlen(line);
		toks.clear();
		while (start < len) {
			str_pos = strchr(line+start,'#');
			if (str_pos == NULL)
				pos = len;
			else
				pos = str_pos - line;
			tmp_char = line[pos];
			line[pos] = 0;
			toks.push_back(line+start);
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

LeafNode* WordArchive::add_special_entry(strid tok)
{
	NodeRef noderef(new LeafNode);
	LeafNode *leaf = (LeafNode*)noderef.get();
	vector<strid> toks;
	toks.push_back(tok);
	leaf->set_id(toks);
	leaf->set_mask(MAIN_LEAF);
	get_root()->add(sarch["<leaf>"],noderef);
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
		VocabIndex id = sarch[get_dic_syllable(s)];
		syllables.push_back(id);
		start = pos+1;
	}

	vector<strid> path = syllables;
	BranchNode* branch = get_root()->add_path(path);
	NodeRef noderef(new LeafNode);
	LeafNode *leaf = (LeafNode*)noderef.get();
	leaf->set_mask(MAIN_LEAF);
	branch->add(sarch["<leaf>"],noderef);
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
		VocabIndex id = sarch[get_dic_syllable(s)];
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
		VocabIndex id = sarch[get_dic_syllable(s)];
		real_syllables.push_back(id);
		start = pos+1;
	}

	vector<strid> path = syllables;
	BranchNode* branch = get_root()->add_path(path);
	NodeRef noderef(new LeafNode);
	LeafNode *leaf = (LeafNode*)noderef.get();
	leaf->set_mask(CASE_LEAF);
	branch->add(sarch["<leaf>"],noderef);
	leaf->set_id(real_syllables);
}

void LeafNode::set_mask(uint maskval,bool mask)
{
	if (mask)
		bitmask |= maskval;
	else
		bitmask &= ~maskval;
}

void LeafNode::set_id(const vector<strid> &_syllables)
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
}

std::ostream& operator << (std::ostream &os,const LeafNode &node)
{
	std::vector<strid> syll;
  node.get_syllables(syll);
	for (std::vector<strid>::size_type i = 0;i < syll.size();i ++) {
		if (i)
			os << " ";
		os << sarch[syll[i]] << "(" << syll[i] << ") ";
	}
	return os;
}

