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
	virtual uint node_begin() const = 0;
	virtual uint node_end() const = 0;
	virtual uint node_count() const = 0;
	virtual const void* node_info(uint node_id) const = 0;
	virtual void get_next(uint node_id,std::vector<uint> &next_id) const = 0;
	/// the return value is undefined if there is no edge between node_from and node_to
	virtual float edge_value(uint node_from,uint node_to) const = 0;
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
	virtual const void* node_info(uint node_id) const;
	virtual void get_next(uint node_id,std::vector<uint> &next_id) const;
	virtual float edge_value(uint node_from,uint node_to) const;
	
};

class Word2DAG : public DAG {
protected:
	WordDAG *dag;

public:
	virtual uint node_begin() const;
	virtual uint node_end() const;
	virtual uint node_count() const;
	virtual const void* node_info(uint node_id) const;
	virtual void get_next(uint node_id,std::vector<uint> &next_id) const;
	virtual float edge_value(uint node_from,uint node_to) const;
};

#endif
