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
		value += (float)count*weight*value;
	}

	we = (WordEntry*)dag->node_info(node_from);
	if (we) {
		uint count = 0;
		for (uint i = 0;i < we->len;i ++)
			if (we->fuzid & (1 << i))
				count ++;
		value += (float)count*weight*value;
	}
	return value;
}

float Penalty2DAG::edge_value(uint node_from,uint node_to) const
{
	float value = dag->edge_value(node_from,node_to);

	WordEntry *we = (WordEntry*)dag->node_info(node_to);
	if (we) {
		float myweight = 0;
		for (uint i = 0;i < we->len;i ++)
			if (we->fuzid & (1 << i))
				myweight += syllable_weights[we->pos+i];
		//if (myweight != 0)
		//cerr << weight*myweight << endl;
		value += (float)weight*myweight*value;
	}

	we = (WordEntry*)dag->node_info(node_from);
	if (we) {
		float myweight = 0;
		for (uint i = 0;i < we->len;i ++)
			if (we->fuzid & (1 << i))
				myweight += syllable_weights[we->pos+i];
		//if (myweight != 0)
		//cerr << weight*myweight << endl;
		value += (float)weight*myweight*value;
	}
	return value > 0 ? value : 0;
}

void Penalty2DAG::set_syllable_weights(const Segmentation &seg)
{
	uint i,n = seg.size();
	strid v[6],ov;

	syllable_weights.resize(seg[n-1].pos+seg[n-1].len);

	for (i = 0;i < n;i ++) {
		v[4] = i >= 2 ? seg[i-2].node.node->get_id() : get_id(START_ID);
		v[3] = i >= 1 ? seg[i-1].node.node->get_id() : get_id(START_ID);
		v[2] = seg[i].node.node->get_id();
		v[1] = i + 1 < n ? seg[i+1].node.node->get_id() : get_id(STOP_ID);
		v[0] = i + 2 < n ? seg[i+2].node.node->get_id() : get_id(STOP_ID);

		v[5] = 0;
		double v1 = LogPtoProb(get_ngram().wordProb(v[2],&v[3]));

		ov = v[4];
		v[4] = 0;
		double v2 = LogPtoProb(get_ngram().wordProb(v[1],&v[2]));
		v[4] = ov;

		ov = v[3];
		v[3] = 0;
		double v3 = LogPtoProb(get_ngram().wordProb(v[0],&v[1]));
		v[3] = ov;

		for (uint j = seg[i].pos;j < seg[i].pos+seg[i].len;j ++)
			syllable_weights[j] = (v1+v2+v3)/3;
	}
}
