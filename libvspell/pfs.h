#ifndef __PFS_H__ // -*- tab-width: 2 mode: c++ -*-
#define __PFS_H__

#ifndef __DAG_H__
#include "dag.h"
#endif

/**
   Priority-First Search.
   This is the PFS approach to the shortest path problem in graph.
 */

class PFS
{
public:
	//	void segment_best(const Lattice &words,Segmentation &seps);
	void search(const DAG &dag,Path &seps);
};

#endif
