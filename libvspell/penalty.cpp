#include "penalty.h"					// -*- tab-width: 2 -*-
#include "spell.h"

//using namespace Dictionary;
using namespace std;

float PenaltyDAG::edge_value(uint node_from,uint node_to) const
{
	float value = dag->edge_value(node_from,node_to);

	WordEntry *we = (WordEntry*)dag->node_info(node_to);
	if (we) {
		uint count = 0;
		for (uint i = 0;i < we->len;i ++)
			if (we->fuzid & (1 << i))
				count ++;
		value -= (float)count*weight*value;
	}

	we = (WordEntry*)dag->node_info(node_from);
	if (we) {
		uint count = 0;
		for (uint i = 0;i < we->len;i ++)
			if (we->fuzid & (1 << i))
				count ++;
		value -= (float)count*weight*value;
	}
	return value > 0 ? value : 0;
}
