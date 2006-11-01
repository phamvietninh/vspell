#include "stridtree.h"

using namespace std;

void BranchNode::get_leaves(std::vector<LeafNode*> &_nodes) const
{
	const_np_range range;
	range = nodes.equal_range(sarch["<leaf>"]);
	node_map::const_iterator iter;
	for (iter = range.first;iter != range.second; ++iter)
		if (iter->second->is_leaf())
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

		nr_lines ++;
	}

	return true;
}

void WordArchive::add_special_entry(strid tok)
{
	NodeRef noderef(new LeafNode);
	LeafNode *leaf = (LeafNode*)noderef.get();
	leaf->set_id(tok);
	get_root()->add(sarch["<leaf>"],noderef);
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
	branch->add("<leaf>",noderef);

	// reconstruct word id
	string word;
	int i,nr_syllables = syllables.size();
	for (i = 0;i < nr_syllables;i ++) {
		if (i)
			word += "_";
		word += sarch[syllables[i]];
	}
	leaf->set_id(sarch[word]);

}
