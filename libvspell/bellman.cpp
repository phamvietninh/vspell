#include "bellman.h"		// -*- tab-width: 2 -*-

using namespace std;

void Bellman::search(const DAG &dag,Path &seps)
{
	vector<uint> last;
	vector<bool> seen;
	vector<float> length;					// examining nodes
	uint i,n,ii,nn,v;

	n = dag.node_count();
	length.resize(n);
	last.resize(n);
	seen.resize(n);

	length[dag.node_begin()] = 0;
	seen[dag.node_begin()] = true;
	last[dag.node_begin()] = dag.node_begin();

	vector<uint> nexts;
	vector<pair<uint,uint> > edges;
	uint l;
	i = 0;
	nexts.clear();
	nexts.push_back(dag.node_begin());
	while (i < (l = nexts.size())) {
		v = nexts[i++];
		dag.get_next(v,nexts);
		nn = nexts.size();
		for (ii = l;ii < nn;ii ++)
			edges.push_back(make_pair(v,nexts[ii]));
	}


	for (bool cont = true,k = 0; cont && k < n;k ++) {
		cont = false;
		nn = edges.size();
		for (ii = 0;ii < nn;ii ++) {
			i = edges[ii].first;
			v = edges[ii].second;
			if (seen[i] && 
					(!seen[v] || length[v] > length[i] + dag.edge_value(i,v))) {
				length[v] = length[i] + dag.edge_value(i,v);
				last[v] = i;
				seen[v] = true;
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
