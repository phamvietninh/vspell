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
		 new WordNode(get_sarch()["<root>"]) : 
		 new FuzzyWordNode(get_sarch()["<root>"]));

	cerr << "Loading... ";
	//str = (boost::format("wordlist.%s") % oldres).str().c_str();
	str = "wordlist";
	get_root()->load(str);
	str = (boost::format("ngram.%s") % oldres).str().c_str();
	File f(str,"rt",0);
	if (!f.error())
		get_ngram().read(f);
	else
		cerr << "Ngram loading error..." << endl;
	cerr << "done" << endl;

	get_sarch().set_blocked(true);

	//wfst.set_wordlist(get_root());

	string s;
	int i,ii,iii,n,nn,nnn,z;
	int count = 0;
	NgramStats stats(get_sarch().get_dict(),3);
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
			//cerr << st << endl; 
			Lattice words;
			words.construct(st);
			//cerr << words << endl;
			Segmentation seg(words.we);
			PFS wfst;
			/* // pfs don't distinguish
			if (nofuz2)
				wfst.segment_best_no_fuzzy(words,seg);
			else
				wfst.segment_best(words,seg);
			*/
			Path path;
			WordDAG dag(&words);
			wfst.search(dag,path);
			seg.resize(path.size()-2);
			copy(path.begin()+1,path.end()-1,seg.begin());

			//seg.pretty_print(cout,st) << endl;

			VocabIndex *vi;
			n = st.get_syllable_count();
			vi = new VocabIndex[n+1];
			vi[n] = Vocab_None;
			for (i = 0;i < n;i ++)
				vi[i] = st[i].get_cid();
			syllable_stats.countSentence(vi);
			delete[] vi;
			n = path.size();
			if (n > 3) {
				vi = new VocabIndex[n+1];
				vi[n] = Vocab_None;
				for (i = 0;i < n;i ++) {
					if (path[i] == dag.node_begin())
						vi[i] = get_id(START_ID);
					else if (path[i] == dag.node_end())
						vi[i] = get_id(STOP_ID);
					else
						vi[i] = ((WordEntry*)dag.node_info(path[i]))->node.node->get_id();
					//cerr << "<" << sarch[vi[i]] << "> ";
				}
				//cerr << endl;
				//cerr << n << endl;
				stats.countSentence(vi);
				//cerr << "done" << endl;
				delete[] vi;
			}

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
	get_ngram().estimate(stats);
	get_syngram().estimate(syllable_stats);
	//wfst.enable_ngram(true);

	cerr << "Saving... ";
	//str = (boost::format("wordlist.wl.%s") % newres).str().c_str();
	//get_root()->save(str);
  
	str = (boost::format("ngram.%s") % newres).str().c_str();
	File ff(str,"wt");
	get_ngram().write(ff);
	str = (boost::format("syngram.%s") % newres).str().c_str();
	File fff(str,"wt");
	get_syngram().write(fff);
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

