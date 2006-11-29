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
	bool record_sc = false;
	bool replay_sc = false;
	const char *str;

	int argi = 1;
	argc --;

	if (argc >= 1 && !strcmp(argv[argi],"--record")) {
		record_sc = true;
		argc --;
		argi ++;
	}

	if (argc >= 1 && !strcmp(argv[argi],"--replay")) {
		replay_sc = true;
		argc --;
		argi ++;
	}

	if (argc >= 1) {
		str = argv[argi];
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
	if (replay_sc) {
		while (!feof(stdin)) {
			if (++count % 200 == 0)
				fprintf(stderr,"%d\n",count);
			if (SoftCounter::replay2(stdin,stdout,count-1,first_count) == -2)
				break;
		}
		return 0;
	}
	while (!cin.eof()) {
		if (++count % 200 == 0)
			cerr << count << endl;
		Lattice lat;
		cin >> lat;
		WordDAG dagw(&lat);
		DAG *dag = &dagw;
		WordDAG2 *dagw2;
		if (trigram && !record_sc) {
			dagw2 = new WordDAG2(&dagw);
			dag = dagw2;
		}
		//sc.count(words,stats);
		if (record_sc) {
			SoftCounter::record2(*dag,stdout,count-1);
		}
		else
			SoftCounter::count_dag(*dag,cout,count-1,first_count);
		if (trigram && !record_sc)
			delete (WordDAG2*)dag;
	}
	return 0;
}
