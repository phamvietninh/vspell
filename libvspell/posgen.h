#ifndef __POSGEN_H__ // -*- tab-width: 2 mode: c++ -*-
#define __POSGEN_H__

#ifndef __SPELL_H__
#include "spell.h"
#endif

#ifndef __VECTOR__
#include <vector>
#endif

class PosGen
{
private:
	uint ii,n;
	int i;
	std::vector<uint> pos;
	bool run;

	uint len;

public:
	/**
		 initialization C(n,k)
		 len_: n
		 n_: k
	 */
	void init(uint len_,uint n_);

	/**
		 On each call, it fills pos with positions. 
		 Notice that pos.size() may be larger than len.
		 But len is used instead pos.size().
	 */
	bool step(std::vector<uint> &pos,uint &len);
	void done();
};


/**
	 Position generator.
 */

class Generator
{
private:
	PosGen pgen;

public:
	void init(const Sentence &st);
	bool step(std::vector<uint> &pos,uint &len);
	void done();
};

#endif
