#include "posgen.h"		// -*- tab-width: 2 mode: c++ -*-

using namespace std;

void Generator::init(const Sentence &_st)
{
	int nr_misspelled,len;
	nr_misspelled = 3;		// REMEMBER THIS NUMBER
	len = _st.get_syllable_count();
	pgen.init(len,nr_misspelled);
}

void Generator::done()
{
	pgen.done();
}

/**
	Generate every possible 3-misspelled-positions.
	Then call WFST::generate_misspelled_words.
 */

bool Generator::step(vector<uint> &_pos,uint &_len)
{
	return pgen.step(_pos,_len);
}

void PosGen::init(uint len_,uint n_)
{
	n = n_;
	len = len_;

	pos.resize(n);
	run = true;

	// initialize
	for (i = 0;i < n;i ++)
		pos[i] = i;

	i = 0;
}

void PosGen::done()
{
}

/**
	Generate every possible 3-misspelled-positions.
	Then call WFST::generate_misspelled_words.
 */

bool PosGen::step(vector<uint> &_pos,uint &_len)
{
	while (run) {

		// move to the next counter
		if (i+1 < n && pos[i] < len) {
			i ++;

			// do something here with misspelled_pos[i]
			_pos = pos;
			_len = i;
			return true;
		}

		// the last counter is not full
		if (pos[i] < len) {
			// do something here with misspelled_pos[nr_misspelled]
			_pos = pos;
			_len = n;
			pos[i] ++;
			return true;
		}

		// the last counter is full, step back
		while (i >= 0 && pos[i] == len) i --;
		if (i < 0)
			run = false;
		else {
			pos[i] ++;
			for (ii = i+1;ii < n;ii ++)
				pos[ii] = pos[ii-1]+1;
		}
	}
	return false;
}

