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

class DAG2 : public DAG {
protected:
	WordDAG *dag;
	struct Node {
		uint n1,n2;									// refer to node_id in dag
		uint id;										// refer to nodes
		bool operator < (const Node &n) const {
			return n1 != n.n1 ? n1 < n.n1 : n2 < n.n2;
		}
		friend bool node_cmp (const Node &n1,const Node &n2) {
			return n1.n1 < n2.n1;
		}

	};
	typedef vector<Node> Nodes;
	Nodes nodes;

public:
	DAG2(WordDAG *dag_);
	virtual uint node_begin() const { return nodes.size(); }
	virtual uint node_end() const { return nodes.size()+1; }
	virtual uint node_count() const { return nodes.size()+2; }
	virtual const void* node_info(uint node_id) const;
	virtual void get_next(uint node_id,std::vector<uint> &next_id) const;
	virtual float edge_value(uint node_from,uint node_to) const;
};

DAG2::DAG2(WordDAG *dag_):dag(dag_)
{
	vector<bool> done;
	vector<uint> nexts;
	uint l,i,v,n,ii,nn;

	done.resize(dag->node_count());
	nexts.clear();
	nexts.push_back(dag->node_begin());
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
		}
	}
	done.clear();

	n = nodes.size();
	for (i = 0;i < n;i ++) {
		pop_heap(nodes.begin()+i,nodes.end());
		nodes[i].id = i;
	}
	
}

void DAG2::get_next(uint node_id,std::vector<uint> &next_id) const
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

float DAG2::edge_value(uint node_from,uint node_to) const
{
	uint n = nodes.size();
	VocabIndex v[3],v2;
	v[2] = Vocab_None;

	if (node_from < n && node_to < n) {
		if (nodes[node_from].n2 != nodes[node_to].n1)
			return 1000;							// no edge between these nodes
		v[0] = ((WordEntry*)dag->node_info(nodes[node_from].n1))->node.node->get_id();
		v[1] = ((WordEntry*)dag->node_info(nodes[node_from].n2))->node.node->get_id();
		v2   = ((WordEntry*)dag->node_info(nodes[node_to  ].n2))->node.node->get_id();
	} else if (node_from == node_begin() && node_to < n) {
		v[0] = get_id(START_ID);
		v[1] = ((WordEntry*)dag->node_info(nodes[node_to].n1))->node.node->get_id();
		v2   = ((WordEntry*)dag->node_info(nodes[node_to].n2))->node.node->get_id();
	} else if (node_to == node_end() && node_from < n) {
		v[0] = ((WordEntry*)dag->node_info(nodes[node_from].n1))->node.node->get_id();
		v[1] = ((WordEntry*)dag->node_info(nodes[node_from].n2))->node.node->get_id();
		v2   = get_id(START_ID);
	} else
		return 1000;

	return (-get_ngram().wordProb(v2,v));
}

const void* DAG2::node_info(uint node_id) const
{
	return (node_id < nodes.size()) ?  dag->node_info(nodes[node_id].n2) : NULL;
}
