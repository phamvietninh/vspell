// -*- tab-width: 2 mode: c++ -*-

#ifndef __CGEN_H__
#define __CGEN_H__

#ifndef __SPELL_H__
#include "spell.h"
#endif

#ifndef __VECTOR__
#include <vector>
#endif

/**
	 Combination Generator.
	 It is used to generate combinations ;)
 */


class CGen
{
private:
	std::vector<uint> limit,iter;
	int nr_limit,cur;

public:
	void init(const std::vector<uint> &limit);
	bool step(std::vector<uint> &pos);
	void done();
};

#endif
