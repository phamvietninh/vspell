#ifndef __BELLMAN_H__ // -*- tab-width: 2 mode: c++ -*-
#define __BELLMAN_H__

#ifndef __DAG_H__
#include "dag.h"
#endif

/**
   Bellman shortest path search.
   The reason to use Bellman-Ford instead of PFS is to be able to find
   the shortest path  with negative weight. Negative weights is made
   from graph normalization process.
 */

class Bellman
{
public:
	void search(const DAG &dag,Path &seps);
};

#endif
