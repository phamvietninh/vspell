#include "softcount.h"		// -*- tab-width: 2 -*-

using namespace std;

/**
	 No short description.
 */
void SoftCounter::count(const Lattice &w,NgramFractionalStats &stats)
{
	vector<float> Sleft,Sright;
	vector<vector<uint> > prev;
	//boost::shared_ptr<WordEntries> p_we = w.we;
	//const WordEntries &we = *p_we;
	int i,n = w.get_word_count(),v,vv;
	float sum = 0;
	VocabIndex vi[3];
	vector<uint> ends;
	float fc;

	Sleft.resize(w.we->size());
	Sright.resize(w.we->size());
	prev.resize(w.we->size());

	vi[1] = Vocab_None;

	// first pass: Sleft
	for (i = 0;i < n;i ++) {
		const WordEntryRefs &wers = w.get_we(i);
		int ii,nn = wers.size();
		float add;
		for (ii = 0;ii < nn;ii ++) {
			// wers[ii] is the first node (W).
			v = wers[ii]->id;

			if (i == 0) {
				vi[0] = start_id;
				Sleft[v] = -ngram.wordProb((*w.we)[v].node.node->get_id(),vi);
			}

			int next = wers[ii]->pos+wers[ii]->len;
			if (next < n) {
				const WordEntryRefs &wers2 = w.get_we(next);
				int iii,nnn = wers2.size();
				for (iii = 0;iii < nnn;iii ++) {
					// wers2[iii] is the second node (W').
					vv = wers2[iii]->id;
					vi[0] = (*w.we)[v].node.node->get_id();
					add = Sleft[v]*(-ngram.wordProb((*w.we)[vv].node.node->get_id(),vi));
					Sleft[vv] += add;
					
					// init prev references for Sright phase
					prev[vv].push_back(v);
				}
			} else {
				vi[0] = (*w.we)[v].node.node->get_id();
				Sright[v] = -ngram.wordProb(stop_id,vi);
				sum += Sleft[v];
				ends.push_back(v);
			}
		}
	}

	// second pass: Sright
	Sright[w.we->size()-1] = 1;
	for (i = n-1;i >= 0;i --) {
		const WordEntryRefs &wers = w.get_we(i);
		int ii,nn = wers.size();
		float add;
		for (ii = 0;ii < nn;ii ++) {
			// wers[ii] is the first node (W).
			v = wers[ii]->id;

			int iii,nnn = prev[v].size();
			for (iii = 0;iii < nnn;iii ++) {
				// vv is the second node (W').
				vv = prev[v][iii];
				vi[0] = (*w.we)[vv].node.node->get_id();
				add = Sright[v]*(-ngram.wordProb((*w.we)[v].node.node->get_id(),vi));
				Sright[vv] += add;

				// collect fractional counts
				fc = Sleft[vv]*add/sum; // P(v/vv)
				vi[0] = vv;
				vi[1] = v;
				vi[2] = Vocab_None;
				*stats.insertCount(vi) += fc;
				vi[1] = Vocab_None;
			}
		}
	}

	// collect fc of ends
	// we can use Sright with no problems becase there is only one edge
	// from ends[i] to the end.
	n = ends.size();
	vi[2] = Vocab_None;
	vi[1] = stop_id;
	for (i = 0;i < n;i ++) {
		fc = Sleft[ends[i]]*Sright[ends[i]]/sum; 
		vi[0] = ends[i];
		*stats.insertCount(vi) += fc;
	}

	vi[0] = start_id;
	const WordEntryRefs &wers = w.get_we(0);
	n = wers.size();
	for (i = 0;i < n;i ++) {
		fc = Sleft[wers[i]->id]*Sright[wers[i]->id]/sum;
		vi[1] = wers[i]->id;
		*stats.insertCount(vi) += fc;
	}
}

