// -*- tab-width: 2 -*-
#include "pfs.h"
#include "distance.h"
#include <string>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <iostream>
#include "sentence.h"
#include <boost/format.hpp>

using namespace std;

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
	const char *str;

	dic_init(nofuz ? 
		 new WordNode(get_sarch()["<root>"]) : 
		 new FuzzyWordNode(get_sarch()["<root>"]));

	cerr << "Loading... ";
	str = "wordlist";
	get_root()->load(str);
	cerr << "done" << endl;

	get_sarch().set_blocked(true);

	string s;
	int i,ii,iii,n,nn,nnn,z;
	int count = 0;
	NgramStats syllable_stats(get_sarch().get_dict(),2);
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

			VocabIndex *vi;
			n = st.get_syllable_count();
			vi = new VocabIndex[n+1];
			vi[n] = Vocab_None;
			for (i = 0;i < n;i ++)
				vi[i] = get_sarch().in_dict(st[i].get_cid()) ? st[i].get_cid() : get_id(UNK_ID);
			syllable_stats.countSentence(vi);
			delete[] vi;
		}
	}

	cerr << "Calculating... ";
	get_syngram().estimate(syllable_stats);
	cerr << "Saving... ";
	str = (boost::format("syngram.%s") % newres).str().c_str();
	File fff(str,"wt");
	get_syngram().write(fff);
	cerr << endl;
	return 0;
}

