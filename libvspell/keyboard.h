// -*- tab-width:2 mode:c++ -*-
#ifndef __SPELL_H__
#include "spell.h"
#endif

#ifndef __CGEN_H__
#include "cgen.h"
#endif

#ifndef __POSGEN_H__
#include "posgen.h"
#endif

#ifndef __STRING__
#include <string>
#endif

#ifndef __VECTOR__
#include <vector>
#endif

#ifndef __SET__
#include <set>
#endif

class KeyRecover
{
private:
	std::string input;
	std::vector<uint> vvv;
	std::vector<const char*> vmap;
	PosGen posgen;
	CGen cgen;
	std::vector<uint> v;
	uint len;
	bool inner_loop;

	bool inner_step(std::string &output);

public:
	void init(const char *input,int N = 2);
	bool step(std::string &output);
	void done();
};

void keyboard_recover(const char *input,std::set<std::string> &output);
void vni_recover(const char *input,std::set<std::string> &output);
void telex_recover(const char *input,std::set<std::string> &output);
