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

void estimate(Ngram &ngram,NgramFractionalStats &stats);

int main(int argc,char **argv)
{
	if (argc < 3) {
		fprintf(stderr,"Need at least 2 argument.\n");
		return 0;
	}
  
	char *oldres = argv[1];
	char *newres = argv[2];
	bool nofuz = true;
	bool nofuz2 = true;
	bool trigram = true;
	const char *str;

	dic_init();

	cerr << "Loading... ";
	str = "wordlist";
	warch.load(str);
	str = (boost::format("ngram.%s") % oldres).str().c_str();
	File f(str,"rt",0);
	if (!f.error())
		get_ngram().read(f);
	else
		cerr << "Ngram loading error..." << endl;
	cerr << "done" << endl;

	get_sarch().set_blocked(true);

	string s;
	int i,ii,iii,n,nn,nnn,z;
	int count = 0;
	NgramFractionalStats stats(get_sarch().get_dict(),3);
	while (getline(cin,s)) {
		count ++;
		if (count % 200 == 0)
			cerr << count << endl;
		if (s.empty())
			continue;
		vector<string> ss;
		sentences_split(s,ss);
		for (z = 0;z < ss.size();z ++) {
			Sentence st(ss[z]);
			st.standardize();
			st.tokenize();
			if (!st.get_syllable_count())
				continue;
			//cerr << ">>" << count << endl;
			Lattice words;
			set<WordEntry> wes;
			WordStateFactories factories;
			ExactWordStateFactory exact;
			LowerWordStateFactory lower;
			//FuzzyWordStateFactory fuzzy;
			factories.push_back(&exact);
			factories.push_back(&lower);
			//factories.push_back(&fuzzy);
			words.pre_construct(st,wes,factories);
			mark_proper_name(st,wes);
			words.post_construct(wes);
			//cerr << words << endl;
			WordDAG dagw(&words);
			DAG *dag = &dagw;
			WordDAG2 *dagw2;
			if (trigram) {
				dagw2 = new WordDAG2(&dagw);
				dag = dagw2;
			}
			SoftCounter sc;
			//sc.count(words,stats);
			sc.count(*dag,stats);
			if (trigram)
				delete (WordDAG2*)dag;
		}
	}

	cerr << "Calculating... ";
	//estimate(ngram,stats);
	get_ngram().estimate(stats,NULL);
	//wfst.enable_ngram(true);

	cerr << "Saving... ";
	str = (boost::format("ngram.%s") % newres).str().c_str();
	File ff(str,"wt");
	get_ngram().write(ff);
	cerr << endl;
	/*
	  for (int i = 0;i < 50;i ++) {
	  ostringstream oss;
	  oss << "log." << i;
	  ofstream ofs(oss.str().c_str());
	  cerr << "Iteration " << i << "... ";
	  iterate(ofs,i);
	  cerr << "done" << endl;
	  }
	*/
	return 0;
}

void estimate(Ngram &ngram,NgramFractionalStats &stats)
{
	/*
	 * If no discount method was specified we do the default, standard
	 * thing. Good Turing discounting with the specified min and max counts
	 * for all orders.
	 */
	unsigned order = get_ngram().setorder(0);
	Discount *discounts[order];
	unsigned i;
	Boolean error = false;
	
	for (i = 1; !error & i <= order; i++) {
		discounts[i-1] = new GoodTuring(GT_defaultMinCount, GT_defaultMaxCount);
		/*
		 * Transfer the LMStats's debug level to the newly
		 * created discount objects
		 */
		discounts[i-1]->debugme(stats.debuglevel());

		if (!discounts[i-1]->estimate(stats, i)) {
	    std::cerr << "failed to estimate GT discount for order " << i + 1
								<< std::endl;
	    error = true;
		}
	}
	
	if (!error) {
		error = !get_ngram().estimate((NgramCounts<FloatCount>&)stats, discounts);
	}
	
	for (i = 1; i <= order; i++) {
		delete discounts[i-1];
	}
}

