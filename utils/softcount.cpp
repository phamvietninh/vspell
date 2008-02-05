#include "softcount.h"		// -*- tab-width: 2 -*-
#include <deque>
#include <boost/format.hpp>

/**
   Soft counter.
   Based on the article "Discovering Chinese Words from Unsegmented Text".
   The idea is that we count all possible words in a sentence with a fraction count 
   instead of just count the words of the best segmentation in the sentence (count=1)
 */

using namespace std;

/**
	 No short description.
 */
ostream& SoftCounter::count_lattice(const Lattice &w, ostream &os, bool first_count)
{
	vector<long double> Sleft,Sright;
	vector<vector<uint> > prev;
	//boost::shared_ptr<WordEntries> p_we = w.we;
	//const WordEntries &we = *p_we;
	int i,n = w.get_word_count(),v,vv;
	long double sum = 0;
	VocabIndex vi[3];
	vector<uint> ends;
	long double fc;

	Sleft.resize(w.we->size());
	Sright.resize(w.we->size());
	prev.resize(w.we->size());

	vi[1] = 0;

	// first pass: Sleft
	for (i = 0;i < n;i ++) {
		const WordEntryRefs &wers = w.get_we(i);
		int ii,nn = wers.size();
		long double add;
		for (ii = 0;ii < nn;ii ++) {
			// wers[ii] is the first node (W).
			v = wers[ii]->id;

			if (i == 0) {
				vi[0] = get_id(START_ID);
				Sleft[v] = first_count ? 1 : -(long double)get_ngram().wordProb((*w.we)[v].node.node->get_id(),vi);
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
					add = first_count ? 1 : Sleft[v]*(-(long double)get_ngram().wordProb((*w.we)[vv].node.node->get_id(),vi));
					Sleft[vv] += add;
					
					//cerr << "Sleft("  << vi[0] << "," << vv << ") = " << Sleft[vv] << endl;

					// init prev references for Sright phase
					prev[vv].push_back(v);
				}
			} else {
				vi[0] = (*w.we)[v].node.node->get_id();
				Sright[v] = first_count ? 1 : -(long double)get_ngram().wordProb(get_id(STOP_ID),vi);
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
		long double add;
		for (ii = 0;ii < nn;ii ++) {
			// wers[ii] is the first node (W).
			v = wers[ii]->id;

			int iii,nnn = prev[v].size();
			for (iii = 0;iii < nnn;iii ++) {
				// vv is the second node (W').
				vv = prev[v][iii];
				vi[0] = (*w.we)[vv].node.node->get_id();
				add = first_count ? 1 : Sright[v]*(-(long double)get_ngram().wordProb((*w.we)[v].node.node->get_id(),vi));
				Sright[vv] += add;

				//cerr << "Sright("  << vi[0] << "," << vv << ") = " << Sright[vv] << endl;

				// collect fractional counts
				fc = Sleft[vv]*add/sum; // P(v/vv)
				vi[0] = (*w.we)[vv].node.node->get_id();
				vi[1] = (*w.we)[v].node.node->get_id();
				vi[2] = 0;
				//*stats.insertCount(vi) += fc;
				os << boost::format("%s %s %f\n") % get_ngram()[vi[0]] % get_ngram()[vi[1]] << fc;
				//cerr << "Gram "  << vi[0] << "," << vi[1] << "+=" << fc << endl;
				vi[1] = 0;
			}
		}
	}

	// collect fc of ends
	// we can use Sright with no problems becase there is only one edge
	// from ends[i] to the end.
	n = ends.size();
	vi[2] = 0;
	vi[1] = get_id(STOP_ID);
	for (i = 0;i < n;i ++) {
		fc = Sleft[ends[i]]*Sright[ends[i]]/sum;
		vi[0] = (*w.we)[ends[i]].node.node->get_id();
		//cerr << "Gram "  << vi[0] << "," << vi[1] << "+=" << fc << endl;
		//*stats.insertCount(vi) += fc;
		os << boost::format("%s %s %f\n") % get_ngram()[vi[0]] % get_ngram()[vi[1]] << fc;
	}

	vi[0] = get_id(START_ID);
	const WordEntryRefs &wers = w.get_we(0);
	n = wers.size();
	for (i = 0;i < n;i ++) {
		fc = Sleft[wers[i]->id]*Sright[wers[i]->id]/sum;
		vi[1] = (*w.we)[wers[i]->id].node.node->get_id();
		//cerr << "Gram "  << vi[0] << "," << vi[1] << "+=" << fc << endl;
		//*stats.insertCount(vi) += fc;
		os << boost::format("%s %s %f\n") % get_ngram()[vi[0]] % get_ngram()[vi[1]] << fc;
	}
	return os;
}


ostream& SoftCounter::count_dag(const DAG &dag,ostream &os,int id, bool first_count)
{
	int n = dag.node_count();
	vector<long double> Sleft(n),Sright(n);
	vector<set<uint> > prev(n);
	int i,v,vv;
	long double add;

	//cerr << "Nodes: " << n << endl;

	vector<bool> mark(n);
	deque<uint> traces;
	Sleft[dag.node_begin()] = 1;
	traces.push_back(dag.node_begin());
	// first pass: Sleft
	while (!traces.empty()) {
		v = traces.front();
		traces.pop_front();
		if (mark[v])
			continue;
		else
			mark[v] = true;
		std::vector<uint> nexts;
		dag.get_next(v,nexts);
		n = nexts.size();

		for (i = 0;i < n;i ++) {
			vv = nexts[i];
			add = Sleft[v]*(first_count ? 1 : LogPtoProb(-dag.edge_value(v,vv)));
			if (add == 0.0 || add == -0.0)
				cerr << boost::format("WARNING: %d: Sleft addition for %d is zero (Sleft[%d] = %Lg, prob=%g)") % id % vv % v % Sleft[v] % LogPtoProb(-dag.edge_value(v,vv)) << endl;
			Sleft[vv] += add;
					
			traces.push_back(vv);

			// init prev references for Sright phase
			prev[vv].insert(v);
		}
	}

	//cerr << "Sleft done" << endl;

	long double fc;
	// second pass: Sright
	long double sum = Sleft[dag.node_end()];
	if (sum == 0.0 || sum == -0.0) {
		cerr << boost::format("WARNING: %d: Sum is zero") % id << endl;
		// Can do nothing more because sum is zero
		return os;
	}
	Sright[dag.node_end()] = 1;
	traces.clear();
	traces.push_back(dag.node_end()); // the last v above
	while (!traces.empty()) {
		vv = traces.front();
		traces.pop_front();
		if (!mark[vv])
			continue;
		else
			mark[vv] = false;
		//cerr << vv << " ";
		set<uint>::iterator iter;
		for (iter = prev[vv].begin();iter != prev[vv].end(); ++iter) {
			v = *iter;
			traces.push_back(v);

			add = Sright[vv]*(first_count ? 1 : LogPtoProb(-dag.edge_value(v,vv)));
			if (add == 0.0 || add == -0.0)
				cerr << boost::format("WARNING: %d: Sright addition for %d is zero") % id % v << endl;
			Sright[v] += add;

			// collect fractional counts
			fc = Sleft[v]*add/sum; // P(vv/v)
			VocabIndex vi[10];
			VocabIndex vvv;
			if (dag.fill_vi(v,vv,vvv,vi,9)) {
				uint t,jn,j;
				for (jn = 0;vi[jn] != 0 && jn < 9; jn ++);
				if (jn < 9 && vi[jn] == 0) {
					for (j = 0;j < jn/2;j ++) {
						t = vi[j];
						vi[j] = vi[jn-j-1];
						vi[jn-j-1] = t;
					}
					vi[jn] = vvv;
					vi[jn+1] = 0;
					//stats.countSentence(vi,/*LogPtoProb(--fc)*/fc);
					//*stats.insertCount(vi) += fc;
					// FIXME
					for (int i_vi = 0; vi[i_vi] != 0; i_vi ++)
						cout << get_ngram()[vi[i_vi]] << " ";
					cout << fc;
					cout << endl;
				}
			}
		}
	}
	//cerr << "Sright done" << endl;
	return os;
}

void SoftCounter::record2(const DAG &dag,FILE *fp,int id)
{
	int n = dag.node_count();
	vector<set<uint> > prev(n);
	int i,v,vv;

	vector<bool> mark(n);
	deque<uint> traces;
	traces.push_back(dag.node_begin());

	fprintf(fp,"%d %d %d\n",n,dag.node_begin(),dag.node_end());

	while (!traces.empty()) {
		v = traces.front();
		traces.pop_front();
		if (mark[v])
			continue;
		else
			mark[v] = true;
		std::vector<uint> nexts;
		dag.get_next(v,nexts);
		n = nexts.size();

		for (i = 0;i < n;i ++) {
			vv = nexts[i];

	    VocabIndex edge_vi[2],edge_v;
			dag.fill_vi(v,vv,edge_v,edge_vi,2);
			fprintf(fp,"L %d %d %s %s\n",v,vv,get_ngram()[edge_vi[0]],get_ngram()[edge_v]);

			traces.push_back(vv);

			// init prev references for Sright phase
			prev[vv].insert(v);
		}
	}

	traces.clear();
	traces.push_back(dag.node_end()); // the last v above
	while (!traces.empty()) {
		vv = traces.front();
		traces.pop_front();
		if (!mark[vv])
			continue;
		else
			mark[vv] = false;

		set<uint>::iterator iter;
		for (iter = prev[vv].begin();iter != prev[vv].end(); ++iter) {
			v = *iter;
			traces.push_back(v);

	    VocabIndex edge_vi[2],edge_v;
			dag.fill_vi(v,vv,edge_v,edge_vi,2);
			fprintf(fp,"R %d %d %s %s\n",v,vv,get_ngram()[edge_vi[0]],get_ngram()[edge_v]);
		}
	}
	fprintf(fp,"E 0 0 none none\n");
}

int SoftCounter::replay2(FILE *fp_in,FILE *fp_out, int id,bool first_count)
{
	int n, node_begin, node_end;

	if (fscanf(fp_in,"%d %d %d\n",&n,&node_begin,&node_end) != 3) {
		fprintf(stderr,"Error: %d: Could not read dag count\n",id);
		return -2;
	}

	vector<long double> Sleft(n),Sright(n);
	int i,v,vv;
	long double add;
	char type[2]; // should not be longer than one
	char str1[100],str2[100];
	int right_mode = 0;
	long double sum;

	//cerr << "Nodes: " << n << endl;

	Sleft[node_begin] = 1;

	while (fscanf(fp_in,"%s %d %d %s %s\n",type,&v,&vv,str1,str2) == 5) {
		if (type[0] == 'E')
			return 0;
		//fprintf(stderr,"Got %s %d %d %s %s\n",type,v,vv,str1,str2);
		VocabIndex edge_vi[2],edge_v;
		edge_v = get_ngram()[str2];
		edge_vi[0] = get_ngram()[str1];
		edge_vi[1] = 0;
		if (type[0] == 'L') {
			add = Sleft[v]*(first_count ? 1 : (long double)LogPtoProb(get_ngram().wordProb(edge_v,edge_vi)));
			if (add == 0.0 || add == -0.0)
				fprintf(stderr,"WARNING: %d: Sleft addition for %d is zero (Sleft[%d] = %Lg, prob=%g %s %s)\n",id,vv,v,Sleft[v],LogPtoProb(get_ngram().wordProb(edge_v,edge_vi)),str1,str2);
			Sleft[vv] += add;
		}
		else {
			if (!right_mode) {
				right_mode = 1;
				sum = Sleft[node_end];
				if (sum == 0.0 || sum == -0.0) {
					fprintf(stderr,"WARNING: %d: Sum is zero\n",id);
				}
				Sright[node_end] = 1;
			}
			if (sum == 0.0 || sum == -0.0)
				continue;
			add = Sright[vv]*(first_count ? 1 : (long double)LogPtoProb(get_ngram().wordProb(edge_v,edge_vi)));
			if (add == 0.0 || add == -0.0)
				fprintf(stderr,"WARNING: %d: Sright addition for %d is zero (%s %s)\n",id,v,str1,str2);
			Sright[v] += add;

			// collect fractional counts
			long double fc = Sleft[v]*add/sum; // P(vv/v)
			fprintf(fp_out,"%s %s %Lg\n",str1,str2,fc);
		}
	}
	return 0;
}

/*
	 A work-around because NgramFractionalStats is still buggy.
*/


ostream& SoftCounter::count_dag_fixed(const DAG &dag,ostream &os,bool first_count)
{
	vector<long double> Sleft,Sright;
	vector<set<uint> > prev;
	int i,n,v,vv;
	long double add;

	n = dag.node_count();
	Sleft.resize(n);
	Sright.resize(n);
	prev.resize(n);
	//cerr << "Nodes: " << n << endl;

	// topo sort
	vector<uint> traces;
	traces.push_back(dag.node_begin());
	int itrace = 0;
	while (itrace < traces.size()) {
		v = traces[itrace++];
		std::vector<uint> nexts;
		dag.get_next(v,nexts);
		n = nexts.size();

		for (i = 0;i < n;i ++) {
			vector<uint>::iterator iter = find(traces.begin(),traces.end(),nexts[i]);
			if (iter != traces.end()) {
				traces.erase(iter);
				if (iter - traces.begin() <= itrace-1)
					itrace --;
			}
			traces.push_back(nexts[i]);
		}
	}
	
	// first pass: Sleft
	uint ntrace = traces.size();
	Sleft[dag.node_begin()] = 1; // log(1) = 0
	for (itrace = 0;itrace < ntrace;itrace ++) {
		v = traces[itrace];
		//cout << " " << v << ":" << Sleft[v];
		std::vector<uint> nexts;
		dag.get_next(v,nexts);
		n = nexts.size();

		for (i = 0;i < n;i ++) {
			vv = nexts[i];
			add = Sleft[v]*dag.edge_value(v,vv);
			//cout << "-" << dag.edge_value(v,vv) << "-" << Sleft[vv] << ">" << vv;
			Sleft[vv] += add;

			// init prev references for Sright phase
			prev[vv].insert(v);
		}
	}

	//cerr << "Sleft done" << endl;

	unsigned int fc;
	// second pass: Sright
	long double sum = Sleft[dag.node_end()];
	if (sum == 0)
		return os;
	//cout << "Sum " << sum << endl;

	traces.clear();
	traces.push_back(dag.node_end());
	itrace = 0;
	while (itrace < traces.size()) {
		vv = traces[itrace++];

		set<uint>::iterator iter;
		for (iter = prev[vv].begin();iter != prev[vv].end(); ++iter) {
			vector<uint>::iterator iiter = find(traces.begin(),traces.end(),*iter);
			if (iiter != traces.end()) {
				traces.erase(iiter);
				if (iiter - traces.begin() <= itrace-1)
					itrace --;
			}
			traces.push_back(*iter);
		}
	}
	
	ntrace = traces.size();
	Sright[dag.node_end()] = 1; // log(1) = 0
	for (itrace = 0;itrace < ntrace;itrace ++) {
		vv = traces[itrace];
		//cout << " " << vv << ":" << Sright[vv];
		set<uint>::iterator iter;
		for (iter = prev[vv].begin();iter != prev[vv].end(); ++iter) {
			v = *iter;
			add = Sright[vv]*dag.edge_value(v,vv);
			//cout << "-" << dag.edge_value(v,vv)<< "-" << Sright[v] << ">" << v;
			Sright[v] += add;

			// collect fractional counts
			fc = 100-(unsigned int)((Sleft[v]*add)*100.0/sum); // P(vv/v)
			//cout << Sleft[v] << "+" << dag.edge_value(v,vv) << Sright[vv]<<  "=("<< (Sleft[v]+add)<< ")"<<((Sleft[v]+add)/sum) <<"_" << fc << endl;
			cerr << v << " " << vv << " " << fc << endl;
			if (fc != 0) {
				VocabIndex vi[10];
				VocabIndex vvv;
				if (dag.fill_vi(v,vv,vvv,vi,9)) {
					uint t,jn,j;
					for (jn = 0;vi[jn] != 0 && jn < 9; jn ++);
					if (jn < 9 && vi[jn] == 0) {
						for (j = 0;j < jn/2;j ++) {
							t = vi[j];
							vi[j] = vi[jn-j-1];
							vi[jn-j-1] = t;
						}
						vi[jn] = vvv;
						vi[jn+1] = 0;
						//stats.countSentence(vi,(unsigned int)(fc*10.0));
						//*stats.insertCount(vi) += fc;
						// FIXME
					}
				}
			}
		}
	}
	cerr << endl;
	//long double sum2 = Sright[dag.node_begin()];
	//cout << sum2 << " " << (traces[ntrace-1] == dag.node_begin()) << " " << (sum2 == sum ? "Ok" : "Failed") << endl;
	//cerr << "Sright done" << endl;
	return os;
}
