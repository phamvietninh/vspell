#ifndef __DAG_H__ // -*- tab-width: 2 mode: c++ -*-
#define __DAG_H__

#ifndef __VECTOR__
#include <vector>
#endif

#ifndef __SPELL_H__
#include "spell.h"
#endif

typedef std::vector<uint> Path;

class DAG {
public:
	virtual VocabIndex node_id(uint id) const = 0;
	virtual uint node_begin() const = 0;
	virtual uint node_end() const = 0;
	virtual uint node_count() const = 0;
	virtual const void* node_info(uint node_id) const = 0;
	virtual void get_next(uint node_id,std::vector<uint> &next_id) const = 0;
	/// the return value is undefined if there is no edge between node_from and node_to
	virtual float edge_value(uint node_from,uint node_to) const = 0;
	virtual bool fill_vi(uint node_from,uint node_to,VocabIndex &v,VocabIndex *vi,int size) const = 0;
};

class WordDAG : public DAG {
protected:
	Lattice *lattice;

public:
	WordDAG(Lattice *l);
	virtual uint node_begin() const {
		return (*lattice->we).size();
	}
	virtual uint node_end() const {
		return (*lattice->we).size()+1;
	}
	virtual uint node_count() const {
		return (*lattice->we).size()+2;
	}
	virtual VocabIndex node_id(uint id) const;
	virtual const void* node_info(uint node_id) const;
	virtual void get_next(uint node_id,std::vector<uint> &next_id) const;
	virtual float edge_value(uint node_from,uint node_to) const;
	virtual bool fill_vi(uint node_from,uint node_to,VocabIndex &v,VocabIndex *vi,int size) const;
	
};

class WordDAG2 : public DAG {
protected:
	WordDAG *dag;
	struct Node {
		uint n1,n2;									// refer to node_id in dag
		uint id;										// refer to nodes
		bool operator < (const Node &n) const {
			return n1 != n.n1 ? n1 < n.n1 : n2 < n.n2;
		}
		friend bool node_cmp (const Node &n1,const Node &n2) {
			return n1.n1 < n2.n1;
		}

	};
	typedef std::vector<Node> Nodes;
	Nodes nodes;

public:
	WordDAG2(WordDAG *dag_);
	WordDAG* get_dag() const { return dag; }
	void node_dag_edge(uint node_id,uint &n1,uint &n2);
	virtual uint node_begin() const { return nodes.size(); }
	virtual uint node_end() const { return nodes.size()+1; }
	virtual uint node_count() const { return nodes.size()+2; }
	virtual void demangle(std::vector<uint> &next_id) const;
	virtual VocabIndex node_id(uint id) const;
	virtual const void* node_info(uint node_id) const;
	virtual void get_next(uint node_id,std::vector<uint> &next_id) const;
	virtual float edge_value(uint node_from,uint node_to) const;
	virtual bool fill_vi(uint node_from,uint node_to,VocabIndex &v,VocabIndex *vi,int size) const;
};

#endif
