// -*- tab-width: 2 -*-
#include "distance.h"
#include <string>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <iostream>
#include "sentence.h"
#include "softcount.h"
#include "propername.h"
#include <boost/format.hpp>

using namespace std;

int main(int argc,char **argv)
{
	bool nofuz = true;
	bool nofuz2 = true;
	bool trigram = false;
	bool first_count = false;
	const char *str;

	if (argc >= 2) {
		str = argv[1];
		if (!get_ngram().read(str)) {
			first_count = true;
			cerr << "Ngram loading error..." << endl;
		}
	}
	else
		first_count = true;
	dic_init();
	warch.load("wordlist");

	get_ngram().set_blocked(true);

	int count = 0;
	while (!cin.eof()) {
		if (++count % 200 == 0) cerr << count << endl;
		Lattice lat;
		cin >> lat;
		WordDAG dagw(&lat);
		DAG *dag = &dagw;
		WordDAG2 *dagw2;
		if (trigram) {
			dagw2 = new WordDAG2(&dagw);
			dag = dagw2;
		}
		//sc.count(words,stats);
		SoftCounter::count_dag(*dag,cout,count-1,first_count);
		if (trigram)
			delete (WordDAG2*)dag;
	}
	return 0;
}
