#include "pfs.h"		// -*- tab-width: 2 -*-

using namespace std;

class HeapValue : public vector<float> {
public:
	bool operator() (int v1,int v2) {
	  return (*this)[v1] > (*this)[v2];
	}
	
};

void PFS::segment_best(const Words &w,Segmentation &seps)
{
	vector<int> back_traces;
	vector<bool> seen;
	vector<int> candidates;
	HeapValue val;

	int i,n = w.get_word_count();

	val.resize(n+1);
	back_traces.resize(n+1);
	seen.resize(n+1);
	candidates.reserve(n+1);

	candidates.push_back(0);
	seen[0] = true;
	val[0] = 0;

	int c = 1;

	while (c <= n) {
		pop_heap(candidates.begin(),candidates.end(),val);
		int v = candidates.back();
		candidates.pop_back();
		if (v == n)
		  continue;
		c++;
		cerr << "got " << v << " " << val[v] << endl;

		const WordEntryRefs &wers = w.get_we(v);
		int vv,ii,nn = wers.size();
		float value,add;
		VocabIndex vi[2];
		vi[1] = Vocab_None;
		vi[0] = start_id;
		for (ii = 0;ii < nn;ii ++) {
		  vv = wers[ii]->pos+wers[ii]->len;
		  //value = val[v]+wers[ii]->node.node->get_prob();
		  vi[0] = (*w.we)[back_traces[v]].node.node->get_id();
		  add = (-ngram.wordProb(wers[ii]->node.node->get_id(),vi));
		  value = val[v]+ add;
		  cerr << "examine " << vv << "(" << wers[ii]->node << ")";
		  if (!seen[vv]) {
				candidates.push_back(vv);
				seen[vv] = true;
				val[vv] = value;
				push_heap(candidates.begin(),candidates.end(),val);
				back_traces[vv] = wers[ii]->id;
				cerr << " add " << val[vv] << "=" << v << "+"<< add;
		  } else {
	if (val[vv] > value) {
		val[vv] = value;
		vector<int>::iterator iter = find(candidates.begin(),candidates.end(),vv);
		while (true) {
		  push_heap(candidates.begin(),iter);
		  if (iter != candidates.end())
		    ++iter;
		  else
		    break;
		}
		back_traces[vv] = wers[ii]->id;
		cerr << " val " << val[vv] << "=" << v << "+"<< add;
	}
		  }
		  cerr << endl;
		}
	}

	int p = n;
	const WordEntries &we = *w.we;
	while (p) {
		int l = we[back_traces[p]].len;
		seps.push_back(back_traces[p]);
		p -= l;
	}
	reverse(seps.begin(),seps.end());

	cerr << "done" << endl;
}
