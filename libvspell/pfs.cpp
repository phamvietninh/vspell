#include "pfs.h"		// -*- tab-width: 2 -*-

using namespace std;

class HeapValue : public vector<float> {
public:
	bool operator() (uint v1,uint v2) {
	  return (*this)[v1] > (*this)[v2];
	}
	
};

/*
void PFS::segment_best(const Lattice &w,Segmentation &seps)
{
	vector<uint> last;
	vector<bool> seen;						// true if a node is seen
	vector<uint> candidates;				// examining nodes
	HeapValue val;

	int n = w.get_word_count();

	seps.we = w.we;

	val.resize(w.we->size()+1);
	last.resize(w.we->size()+1);
	seen.resize(w.we->size()+1);
	candidates.reserve(w.we->size()+1);

	const WordEntryRefs &wers_start = w.get_we(0);
	int ii,nn = wers_start.size();
	for (ii = 0; ii < nn;ii ++) {
		int v = wers_start[ii]->id;
		candidates.push_back(v);
		push_heap(candidates.begin(),candidates.end(),val);
		seen[v] = true;
		val[v] = 0;
		last[v] = v;
	}

	// while there is a node to examine
	while (!candidates.empty()) {

		// get a node
		pop_heap(candidates.begin(),candidates.end(),val);
		int v = candidates.back();
		candidates.pop_back();

		//cerr << "got " << (*w.we)[v].node << " " << val[v] << endl;

		int next = (*w.we)[v].pos+(*w.we)[v].len;

		if (next >= n) {
			int vv = w.we->size();
		  if (!seen[vv]) {
				seen[vv] = true;
				val[vv] = val[v];
				last[vv] = v;
				//cerr << " end " << val[vv] << "=" << v << endl;
		  } else {
				if (val[vv] > val[v]) {
					val[vv] = val[v];
					last[vv] = v;
					//cerr << " new end " << val[vv] << "=" << v << endl;
				}
		  }
			continue;
		}

		const WordEntryRefs &wers = w.get_we(next);
		uint vv;
		nn = wers.size();
		float value,add;
		VocabIndex vi[2];
		vi[1] = Vocab_None;
		vi[0] = (*w.we)[v].node.node->get_id();
		for (ii = 0;ii < nn;ii ++) {
		  vv = wers[ii]->id;
		  //value = val[v]+wers[ii]->node.node->get_prob();
		  //vi[0] = (*w.we)[last[v]].node.node->get_id();
		  add = (-get_ngram().wordProb((*w.we)[vv].node.node->get_id(),vi));
		  value = val[v] + add;
		  //cerr << "examine " << vv << "(" << wers[ii]->node << ")";

		  if (!seen[vv]) {
				candidates.push_back(vv);
				seen[vv] = true;
				val[vv] = value;
				push_heap(candidates.begin(),candidates.end(),val);
				last[vv] = v;
				//cerr << " add " << val[vv] << "=" << v << "+"<< add;
		  } else {
				if (val[vv] > value) {
					val[vv] = value;
					last[vv] = v;

					// re-heap if necessary
					vector<uint>::iterator iter = find(candidates.begin(),candidates.end(),vv);
					while (true) {
						push_heap(candidates.begin(),iter,val);
						if (iter != candidates.end())
							++iter;
						else
							break;
					}
					//cerr << " val " << val[vv] << "=" << v << "+"<< add;
				}
		  }
		  //cerr << endl;
		}
	}

	const WordEntries &we = *w.we;
	uint v = we.size();
	while (true) {
		//cerr << v << "->" << last[v] << endl;
		if (last[v] != v) {
			v = last[v];
			seps.push_back(v);
		} else
			break;
	}
	reverse(seps.begin(),seps.end());

	//cerr << "done" << endl;
}
*/

void PFS::search(const DAG &dag,Path &seps)
{
	vector<uint> last;
	vector<bool> seen;						// true if a node is seen
	vector<uint> candidates;			// examining nodes
	uint v;
	HeapValue val;
	uint n = dag.node_count();

	val.resize(n+1);
	last.resize(n+1);
	seen.resize(n+1);
	candidates.reserve(n+1);

	vector<uint> next_nodes;
	v = dag.node_begin();
	last[v] = v;
	val[v] = 0;
	seen[v] = true;
	candidates.push_back(v);
	int ii,nn = next_nodes.size();
	/*
	dag.get_next(dag.node_begin(),next_nodes);
	int ii,nn = next_nodes.size();
	for (ii = 0; ii < nn;ii ++) {
		v = next_nodes[ii];
		candidates.push_back(v);
		push_heap(candidates.begin(),candidates.end(),val);
		seen[v] = true;
		val[v] = 0;
		last[v] = dag.node_begin();
		}
*/

	// while there is a node to examine
	while (!candidates.empty()) {

		// get a node
		pop_heap(candidates.begin(),candidates.end(),val);
		v = candidates.back();
		candidates.pop_back();

		//cerr << "check " << v << " " << val[v] << endl;
		
		next_nodes.clear();
		dag.get_next(v,next_nodes);

		uint vv;
		nn = next_nodes.size();
		float value,add;
		for (ii = 0;ii < nn;ii ++) {
		  vv = next_nodes[ii];
		  add = dag.edge_value(v,vv);
		  value = val[v] + add;
		  //cerr << "examine " << vv << "(" << vv << ")";

		  if (!seen[vv]) {
				candidates.push_back(vv);
				seen[vv] = true;
				val[vv] = value;
				push_heap(candidates.begin(),candidates.end(),val);
				last[vv] = v;
				//cerr << " add " << val[vv] << "=" << v << "+"<< add;
		  } else {
				if (val[vv] > value) {
					val[vv] = value;
					last[vv] = v;

					// re-heap if necessary
					vector<uint>::iterator iter = find(candidates.begin(),candidates.end(),vv);
					while (true) {
						push_heap(candidates.begin(),iter,val);
						if (iter != candidates.end())
							++iter;
						else
							break;
					}
					//cerr << " val " << val[vv] << "=" << v << "+"<< add;
				}
		  }
		  //cerr << endl;
		}
	}

	v = dag.node_end();
	if (!seen[v])
		return;											// ARGH! NO WAY TO THE END!!!

	do {
		seps.push_back(v);
		v = last[v];
	} while (v != last[v]);
	seps.push_back(v);
	reverse(seps.begin(),seps.end());

	//cerr << "done" << endl;
}
