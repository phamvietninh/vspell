#include "dag.h" // -*- tab-width: 2 mode: c++ -*-

using namespace std;

WordDAG::WordDAG(Lattice *l)
{
	lattice = l;
}

const void* WordDAG::node_info(uint node_id) const
{
	const	WordEntries& we = *lattice->we;
	return node_id < we.size() ? &we[node_id] : NULL;
}

void WordDAG::get_next(uint node_id,vector<uint> &next_id) const
{
	const	WordEntries& we = *lattice->we;

	if (node_id < we.size()) {
		if (we[node_id].pos+we[node_id].len == lattice->get_word_count())
			next_id.push_back(node_end());
		else {
			const WordEntryRefs& wes = lattice->get_we(we[node_id].pos+we[node_id].len);
			int i,n = wes.size();
			for (i = 0;i < n;i ++)
				next_id.push_back(wes[i]->id);
		}
		return;
	}

	if (node_id == node_begin()) {		// begin node
		const WordEntryRefs& wes = lattice->get_we(0);
		int i,n = wes.size();
		for (i = 0;i < n;i ++)
			next_id.push_back(wes[i]->id);
		return;		
	}
}

float WordDAG::edge_value(uint node_from,uint node_to) const
{
	const	WordEntries& we = *lattice->we;
	uint n = we.size();
	if (node_from < n && node_to < n) {
		VocabIndex vi[2];
		vi[1] = 0;
		vi[0] = we[node_from].node.node->get_id();
		return (-get_ngram().wordProb(we[node_to].node.node->get_id(),vi));
	}

	if (node_from == node_begin() && node_to < n) {
		VocabIndex vi[2];
		vi[1] = 0;
		vi[0] = get_id(START_ID);
		return (-get_ngram().wordProb(we[node_to].node.node->get_id(),vi));
	}

	if (node_to == node_end() && node_from < n) {
		VocabIndex vi[2];
		vi[1] = 0;
		vi[0] = we[node_to].node.node->get_id();
		return (-get_ngram().wordProb(get_id(STOP_ID),vi));
	}

	return 1000;
}
