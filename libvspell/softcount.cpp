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
				vi[0] = get_id(START_ID);
				Sleft[v] = -get_ngram().wordProb((*w.we)[v].node.node->get_id(),vi);
				//cerr << "Sleft("  << vi[0] << "," << v << ") = " << Sleft[v] << endl;
			}

			int next = wers[ii]->pos+wers[ii]->len;
			if (next < n) {
				const WordEntryRefs &wers2 = w.get_we(next);
				int iii,nnn = wers2.size();
				for (iii = 0;iii < nnn;iii ++) {
					// wers2[iii] is the second node (W').
					vv = wers2[iii]->id;
					vi[0] = (*w.we)[v].node.node->get_id();
					add = Sleft[v]*(-get_ngram().wordProb((*w.we)[vv].node.node->get_id(),vi));
					Sleft[vv] += add;
					
					//cerr << "Sleft("  << vi[0] << "," << vv << ") = " << Sleft[vv] << endl;

					// init prev references for Sright phase
					prev[vv].push_back(v);
				}
			} else {
				vi[0] = (*w.we)[v].node.node->get_id();
				Sright[v] = -get_ngram().wordProb(get_id(STOP_ID),vi);
				//cerr << "Sright("  << vi[0] << "," << v << ") = " << Sright[v] << endl;
				sum += Sleft[v];
				//cerr << "Sum " << sum << endl;
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
				add = Sright[v]*(-get_ngram().wordProb((*w.we)[v].node.node->get_id(),vi));
				Sright[vv] += add;

				//cerr << "Sright("  << vi[0] << "," << vv << ") = " << Sright[vv] << endl;

				// collect fractional counts
				fc = Sleft[vv]*add/sum; // P(v/vv)
				vi[0] = (*w.we)[vv].node.node->get_id();
				vi[1] = (*w.we)[v].node.node->get_id();
				vi[2] = Vocab_None;
				*stats.insertCount(vi) += fc;
				//cerr << "Gram "  << vi[0] << "," << vi[1] << "+=" << fc << endl;
				vi[1] = Vocab_None;
			}
		}
	}

	// collect fc of ends
	// we can use Sright with no problems becase there is only one edge
	// from ends[i] to the end.
	n = ends.size();
	vi[2] = Vocab_None;
	vi[1] = get_id(STOP_ID);
	for (i = 0;i < n;i ++) {
		fc = Sleft[ends[i]]*Sright[ends[i]]/sum;
		vi[0] = (*w.we)[ends[i]].node.node->get_id();
		//cerr << "Gram "  << vi[0] << "," << vi[1] << "+=" << fc << endl;
		*stats.insertCount(vi) += fc;
	}

	vi[0] = get_id(START_ID);
	const WordEntryRefs &wers = w.get_we(0);
	n = wers.size();
	for (i = 0;i < n;i ++) {
		fc = Sleft[wers[i]->id]*Sright[wers[i]->id]/sum;
		vi[1] = (*w.we)[wers[i]->id].node.node->get_id();
		//cerr << "Gram "  << vi[0] << "," << vi[1] << "+=" << fc << endl;
		*stats.insertCount(vi) += fc;
	}
}

/*
void SoftCounter::count(const DAG &dag,NgramFractionalStats &stats)
{
	vector<float> Sleft,Sright;
	vector<vector<uint> > prev;
	int i,n,v,vv;
	float sum = 0;
	VocabIndex vi[3];
	float fc;

	Sleft.resize(dag.node_count());
	Sright.resize(dag.node_count());
	prev.resize(dag.node_count());

	deque<uint> traces;
	dag.get_next(dag.node_begin(),traces);
	// first pass: Sleft
	while (!traces.empty()) {
		v = traces.first();
		traces.pop_first();
		std::vector<uint> nexts;
		dag.get_next(v,nexts);
		n = nexts.size();

		for (i = 0;i < n;i ++) {
			vv = nexts[i];
			add = Sleft[v]*dag.edge_value(vv,v);
			Sleft[vv] += add;
					
			// init prev references for Sright phase
			prev[vv].push_back(v);
			traces.push_back(vv);
		}
	}

	// second pass: Sright
	Sright[dag.node_count()-1] = 1;
	dag.push_back(dag.node_end()); // the last v above
	while (!traces.empty()) {
		vv = traces.first();
		traces.pop_first();
		n = prev[vv].size();
		for (i = 0;i < n;i ++) {
			// wers[ii] is the first node (W).
			v = prev[vv][i];
			traces.push_back(v);

			add = Sright[v]*dag.edge_value(vv,v);
			Sright[vv] += add;

			// collect fractional counts
			fc = Sleft[vv]*add/sum; // P(v/vv)
			vi[0] = vv;
			vi[1] = v;
			vi[2] = Vocab_None;
			*stats.insertCount(vi) += fc;
		}
	}
}

*/
