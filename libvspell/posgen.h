#ifndef __POSGEN_H__ // -*- tab-width: 2 mode: c++ -*-
#define __POSGEN_H__

#ifndef __SPELL_H__
#include "spell.h"
#endif

#ifndef __VECTOR__
#include <vector>
#endif

/**
	 Position generator.
 */

class Generator
{
private:
	uint ii,i,nr_misspelled;
	std::vector<uint> misspelled_pos;
	bool run;

	uint len;
	SentenceRef st;

public:
	void init(const Sentence &st);
	bool step(std::vector<uint> &pos,uint &len);
	void done();
};


#endif
