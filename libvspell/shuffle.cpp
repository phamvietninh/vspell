// -*- tab-width: 2 mode: c++ -*-
#include "shuffle.h"

void CharInserter::init(const char *input_)
{
	input = input_;
	len = input.size();
	i = i2 = 0;
}

bool CharInserter::step(string &output)
{
	static char *candidates = "qwertyuiopasdfghjklzxcvbnm";
	uint len2 = strlen(candidates);

	if (i > len)
		return false;

	output = input;
	output.insert(output.start()+i,candidates[i2]);
	i2 ++;
	if (i2 == len2) {
		i2 = 0;
		i ++;
	}
	return true;
}

void CharInserter::done()
{
}

void CharEraser::init(const char *input_)
{
	input = input_;
	len = input.size();
	i = 0;
}

bool CharEraser::step(string &output)
{
	if (i >= len)
		return false;

	output = input;
	output.erase(i,1);
	i ++;

	return true;
}

void CharEraser::done()
{
}

void CharTransposer::init(const char *input_)
{
	input = input_;
	len = input.size();
	i = 0;
}

bool CharTransposer::step(string &output)
{
	if (i >= len-1)
		return false;

	char c;
	output = input;
	c = output[i];
	output[i] = output[i+1];
	output[i+1] = c;
	i ++;

	return true;
}

void CharTransposer::done()
{
}

