#include "pfs.h"		// -*- tab-width: 2 -*-

using namespace std;

class HeapValue : public vector<float> {
public:
	bool operator() (uint v1,uint v2) {
	  return (*this)[v1] > (*this)[v2];
	}
	
};

void PFS::segment_best(const Lattice &w,Segmentation &seps)
{
	vector<uint> back_traces;
	vector<bool> seen;						// true if a node is seen
	vector<uint> candidates;				// examining nodes
	HeapValue val;

	int n = w.get_word_count();

	seps.we = w.we;

	val.resize(w.we->size()+1);
	back_traces.resize(w.we->size()+1);
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
		back_traces[v] = v;
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
				back_traces[vv] = v;
				//cerr << " end " << val[vv] << "=" << v << endl;
		  } else {
				if (val[vv] > val[v]) {
					val[vv] = val[v];
					back_traces[vv] = v;
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
		  //vi[0] = (*w.we)[back_traces[v]].node.node->get_id();
		  add = (-get_ngram().wordProb((*w.we)[vv].node.node->get_id(),vi));
		  value = val[v] + add;
		  //cerr << "examine " << vv << "(" << wers[ii]->node << ")";

		  if (!seen[vv]) {
				candidates.push_back(vv);
				seen[vv] = true;
				val[vv] = value;
				push_heap(candidates.begin(),candidates.end(),val);
				back_traces[vv] = v;
				//cerr << " add " << val[vv] << "=" << v << "+"<< add;
		  } else {
				if (val[vv] > value) {
					val[vv] = value;
					back_traces[vv] = v;

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
		//cerr << v << "->" << back_traces[v] << endl;
		if (back_traces[v] != v) {
			v = back_traces[v];
			seps.push_back(v);
		} else
			break;
	}
	reverse(seps.begin(),seps.end());

	//cerr << "done" << endl;
}
