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

VocabIndex WordDAG::node_id(uint id) const
{
	const	WordEntries& we = *lattice->we;
	return id < we.size() ? we[id].node.node->get_id() : 
		get_id(id == node_begin() ? START_ID : STOP_ID);
}

void WordDAG::get_next(uint node_id,vector<uint> &next_id) const
{
	const	WordEntries& we = *lattice->we;

	if (node_id < we.size()) {
		uint pos = we[node_id].pos+we[node_id].len;
		if (pos >= lattice->get_word_count())
			next_id.push_back(node_end());
		else {
			const WordEntryRefs& wes = lattice->get_we(pos);
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
	VocabIndex vi[2],v;
	vi[1] = Vocab_None;
	uint n = we.size();

	if (node_from < n && node_to < n) {
		vi[0] = we[node_from].node.node->get_id();
		v = we[node_to].node.node->get_id();
	} else if (node_from == node_begin() && node_to < n) {
		vi[0] = get_id(START_ID);
		v = we[node_to].node.node->get_id();
	} else if (node_to == node_end() && node_from < n) {
		vi[0] = we[node_from].node.node->get_id();
		v = get_id(STOP_ID);
	} else
		return 1000;
	return (-get_ngram().wordProb(v,vi));
}

WordDAG2::WordDAG2(WordDAG *dag_):dag(dag_)
{
	vector<bool> done;
	vector<uint> nexts;
	uint l,i,v,n,ii,nn;

	done.resize(dag->node_count());
	nexts.push_back(dag->node_begin());
	i = 0;
	while (i < (l = nexts.size())) {
		v = nexts[i++];
		if (done[v])
			continue;
		else
			done[v] = true;
		dag->get_next(v,nexts);
		nn = nexts.size();
		for (ii = l;ii < nn;ii ++) {
			Node node;
			node.n1 = v;
			node.n2 = nexts[ii];
			nodes.push_back(node);
			push_heap(nodes.begin(),nodes.end());
			//cerr << node.n1 << "=" << node.n2 << endl;
		}
	}
	done.clear();

	n = nodes.size();
	for (i = 0;i < n;i ++) {
		pop_heap(nodes.begin(),nodes.end()-i);
		nodes[n-i-1].id = n-i-1;
	}
	//for (i = 0;i < n;i ++) {
	//	cerr << nodes[i].n1 << "-" << nodes[i].n2 << " " << nodes[i].id << endl;
	//}
}

void WordDAG2::get_next(uint node_id,std::vector<uint> &next_id) const
{
	if (node_id == node_end())
		return;

	pair<Nodes::const_iterator,Nodes::const_iterator> p;
	Node node;
	if (node_id < nodes.size()) {
		if (nodes[node_id].n2 == dag->node_end()) {
			next_id.push_back(node_end()); // add node_end.
			return;
		}
		node.n1 = nodes[node_id].n2; // we don't care n2 because node_cmp don't use it
	} else
		node.n1 = dag->node_begin();
	p = equal_range(nodes.begin(),nodes.end(),node,node_cmp);
	for (Nodes::const_iterator i = p.first;i != p.second; ++i)
		next_id.push_back(i->id);
}

float WordDAG2::edge_value(uint node_from,uint node_to) const
{
	uint n = nodes.size();
	WordEntry *we;
	VocabIndex v[3],v2;
	v[2] = Vocab_None;

	if (node_from < n && node_to < n) {
		if (nodes[node_from].n2 != nodes[node_to].n1)
			return 1000;							// no edge between these nodes
		v[1] = dag->node_id(nodes[node_from].n1);
		v[0] = dag->node_id(nodes[node_from].n2);
		v2   = dag->node_id(nodes[node_to  ].n2);
	} else if (node_from == node_begin() && node_to < n) {
		v[1] = get_id(START_ID);
		v[0] = dag->node_id(nodes[node_to].n1);
		v2   = dag->node_id(nodes[node_to].n2);
	} else if (node_to == node_end() && node_from < n) {
		v[1] = dag->node_id(nodes[node_from].n1);
		v[0] = dag->node_id(nodes[node_from].n2);
		v2   = get_id(START_ID);
	} else
		return 1000;

	if (v[0] == v[1] && v[1] == get_id(START_ID))	// back to 2-gram
		v[1] = Vocab_None;
	if (v[1] == v2 && v2 == get_id(STOP_ID)) {
		v[1] = Vocab_None;
		return (-get_ngram().wordProb(v2,v));
	}
	return (-get_ngram().wordProb(v2,v));
}

const void* WordDAG2::node_info(uint node_id) const
{
	return (node_id < nodes.size()) ?  dag->node_info(nodes[node_id].n2) : NULL;
}

VocabIndex WordDAG2::node_id(uint id) const
{
	return (id < nodes.size()) ?  dag->node_id(nodes[id].n2) :
		get_id(id == node_begin() ? START_ID : STOP_ID);
}

void WordDAG2::demangle(vector<uint> &next_id) const
{
	uint i,n = next_id.size();
	for (i = 0;i < n-1;i ++)
		next_id[i] = nodes[next_id[i]].n2;
	next_id.resize(n-1);
}

