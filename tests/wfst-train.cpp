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


//NgramFractionalStats stats(sarch.get_dict(),2);

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
		 new WordNode(sarch["<root>"]) : 
		 new FuzzyWordNode(sarch["<root>"]));

	cerr << "Loading... ";
	//str = (boost::format("wordlist.wl.%s") % oldres).str().c_str();
	str = "wordlist.wl";
	get_root()->load(str);
	str = (boost::format("ngram.%s") % oldres).str().c_str();
	File f(str,"rt",0);
	if (!f.error())
		ngram.read(f);
	else
		cerr << "Ngram loading error..." << endl;
	cerr << "done" << endl;

	sarch.set_blocked(true);

	//wfst.set_wordlist(get_root());

	string s;
	int i,ii,iii,n,nn,nnn,z;
	int count = 0;
	NgramStats stats(sarch.get_dict(),2);
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
			Lattice words;
			words.construct(st);
			//cerr << words << endl;
			Segmentation seg(words.we);
			PFS wfst;
			if (nofuz2)
				wfst.segment_best_no_fuzzy(words,seg);
			else
				wfst.segment_best(words,seg);

			//seg.pretty_print(cout,st) << endl;

			n = seg.size();
			VocabIndex *vi = new VocabIndex[n+3];
			vi[0] = start_id;
			vi[n+1] = stop_id;
			vi[n+2] = Vocab_None;
			for (i = 0;i < n;i ++) {
				vi[i+1] = seg[i].node.node->get_id();
				//cerr << "<" << sarch[vi[i]] << "> ";
			}
			//cerr << endl;
			stats.countSentence(vi);
			delete[] vi;

			/*
			const WordEntries &we = *words.we;
			n = we.size();
			for (i = 0;i < n;i ++) {
				we[i].node.node->inc_b();
			}

			n = seg.size();
			for (i = 0;i < n;i ++)
				seg[i].node.node->inc_a();
			*/
		}
	}

	cerr << "Calculating... ";
	//get_root()->get_next(unk_id)->get_b() = 0;
	//get_root()->recalculate();
	ngram.estimate(stats);
	//wfst.enable_ngram(true);

	cerr << "Saving... ";
	//str = (boost::format("wordlist.wl.%s") % newres).str().c_str();
	//get_root()->save(str);
  
	str = (boost::format("ngram.%s") % newres).str().c_str();
	File ff(str,"wt");
	ngram.write(ff);
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

