#include "cgen.h"

using namespace std;

void CGen::init(const vector<uint> &_limit)
{
	limit = _limit;
	nr_limit = limit.size();
	iter.resize(nr_limit);
	for (uint i = 0;i < nr_limit;i ++)
		iter[i] = 0;
	cur = 0;
}

void CGen::done()
{
}

/**
	Generate every possible 3-misspelled-positions. 
	Then call WFST::generate_misspelled_words.
 */

bool CGen::step(vector<uint> &_pos)
{
	while (cur >= 0) {

		if (cur == nr_limit-1) {
			_pos = iter;
			while (cur >= 0 && iter[cur] == limit[cur]-1)
				cur --;
			if (cur >= 0)
				iter[cur]++;
			return true;
		}

		cur ++;
		if (cur < nr_limit)
			iter[cur] = 0;
	}
	return false;
}

