#include "bellman.h"		// -*- tab-width: 2 -*-

using namespace std;

void Bellman::search(const DAG &dag,Path &seps)
{
	vector<uint> last;
	vector<uint> node_count;
	vector<float> length;					// examining nodes
	uint i,n,ii,nn,v;

	n = dag.node_count();
	length.resize(n);
	last.resize(n);
	node_count.resize(n);					// seen is node_count != 0

	v = dag.node_begin();
	length[v] = 0;
	last[v] = v;
	node_count[v] = 1;						// 1 node from the beginning

	vector<uint> nexts;
	vector<pair<uint,uint> > edges;
	vector<bool> done;
	uint l;
	i = 0;
	done.resize(dag.node_count());
	nexts.clear();
	nexts.push_back(dag.node_begin());
	while (i < (l = nexts.size())) {
		v = nexts[i++];
		if (done[v])
			continue;
		else
			done[v] = true;
		dag.get_next(v,nexts);
		nn = nexts.size();
		for (ii = l;ii < nn;ii ++) {
			edges.push_back(make_pair(v,nexts[ii]));
			//cerr << "Edge " << v << " " << nexts[ii] << endl;
		}
	}
	done.clear();


	uint k;
	bool cont;
	for (cont = true,k = 0; cont && k < n;k ++) {
		cont = false;
		nn = edges.size();
		for (ii = 0;ii < nn;ii ++) {
			i = edges[ii].first;
			v = edges[ii].second;
			if (!node_count[i])
				continue;
			// normalization. If you don't want it, use the below comment instead.
			float val =  length[i] + (dag.edge_value(i,v) - length[i]) / ((float)node_count[i]+1);
			//float val = length[i]+dag.edge_value(i,v);
			if (!node_count[v] || length[v] > val) {
				//float old = length[v];
				length[v] = val;
				last[v] = i;
				node_count[v] = node_count[i]+1;
				//cerr << val << " " << node_count[v] << " " << v << " " << length[v] << " " << last[v] << " (" << endl;
				cont = true;
			}
		}
	}

	v = dag.node_end();
	do {
		seps.push_back(v);
		v = last[v];
	} while (v != last[v]);
	seps.push_back(v);
	reverse(seps.begin(),seps.end());
	//cerr << "done" << endl;
}
