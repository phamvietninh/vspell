// -*- tab-width: 2 mode: c++ -*-

#ifndef __STRING__
#include <string>
#endif

#ifndef __SPELL_H__
#include "spell.h"
#endif

class CharInserter//CharShuffle
{
private:
	std::string input;
	uint i,i2;
	uint len;

public:
	void init(const char *input);
	bool step(std::string &output);
	void done();
};

class CharEraser//CharShuffle
{
private:
	std::string input;
	uint i;
	uint len;

public:
	void init(const char *input);
	bool step(std::string &output);
	void done();
};

class CharTransposer//CharShuffle
{
private:
	std::string input;
	uint i;
	uint len;

public:
	void init(const char *input);
	bool step(std::string &output);
	void done();
};

