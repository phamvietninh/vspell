#ifndef __PENALTY_H__ // -*- tab-width: 2 mode: c++ -*-
#define __PENALTY_H__

#ifndef __DAG_H__
#include "dag.h"
#endif

class PenaltyDAG : public DAG {
protected:
	/// requires that dag->get_info() return a WordEntry*
	DAG *dag;
	float weight;

public:
	PenaltyDAG(DAG *dag_,float weight_):dag(dag_),weight(weight_) {}

	void set_weight(float weight_) {
		weight = weight_;
	}

	float get_weight() const {
		return weight;
	}

	virtual uint node_begin() const {
		return dag->node_begin();
	}
	virtual uint node_end() const {
		return dag->node_end();
	}
	virtual uint node_count() const {
		return dag->node_count();
	}
	virtual const void* node_info(uint node_id) const {
		return dag->node_info(node_id);
	}
	virtual VocabIndex node_id(uint id) const {
		return dag->node_id(id);
	}
	virtual void get_next(uint node_id,std::vector<uint> &next_id) const {
		dag->get_next(node_id,next_id);
	}
	virtual float edge_value(uint node_from,uint node_to) const;
};

class Penalty2DAG : public PenaltyDAG {
protected:
	std::vector<float> syllable_weights;

public:
	Penalty2DAG(DAG *dag_,float weight_):PenaltyDAG(dag_,weight_) {}

	void set_syllable_weights(const Segmentation &seg);

	virtual float edge_value(uint node_from,uint node_to) const;
};

#endif
