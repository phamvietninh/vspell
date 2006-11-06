// -*- coding: viscii -*-
#include "pfs.h"
#include "bellman.h"
#include "distance.h"
#include "sentence.h"
#include <string>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <../libvspell/boost/format.hpp>

using namespace std;

void print_all_words(const Lattice &words);
int main(int argc,char **argv)
{
  bool fuzzy = true;
  bool bellman = false;
  bool threegram = false;

  for (int i = 1;i < argc;i ++) {
    if (!strcmp(argv[i],"nofuzzy")) fuzzy = false;
    else if (!strcmp(argv[i],"bellman")) bellman = true;
    else if (!strcmp(argv[i],"3gram")) threegram = true;
  }
  dic_init(!fuzzy ? 
	   new WordNode(get_sarch()["<root>"]) : 
	   new FuzzyWordNode(get_sarch()["<root>"]));
  ed_init();

  cerr << "Loading... ";
  get_root()->load("wordlist");
  File f("ngram","rt",0);
  if (!f.error())
	  get_ngram().read(f);
  cerr << "done" << endl;

  get_sarch().set_blocked(true);

  get_sarch().dump();

  /*
    cerr << "Saving...";
    get_root()->save("wordlist2.wl");
    cerr << "done" << endl;
  */

  //wfst.set_wordlist(get_root());
  vector<Sentence> sentences;

/*
  Segmentation seg;
  Sentence st("tui v�a m�i mua m�t chi�c ghe");
  st.standardize();
  st.tokenize();
  vector<WFST::WordInfos> words;
  wfst.get_all_words(st,words);

    {
    int i, nn = words.size();
    for (i = 0;i < nn;i ++) {
    int nnn = words[i].size();
    cerr << "From " << i << endl;
    for (int ii = 0;ii < nnn;ii ++) {
    int nnnn = words[i][ii].fuzzy_match.size();
    cerr << "Len " << ii << endl;
    for (int iii = 0;iii < nnnn;iii ++) {
    cerr << sarch[words[i][ii].fuzzy_match[iii].node->get_id()] << " ";
    cerr << words[i][ii].fuzzy_match[iii].node->get_syllable_count() << " ";
    cerr << words[i][ii].fuzzy_match[iii].distance << " ";
    cerr << words[i][ii].fuzzy_match[iii].node->get_prob() << endl;
    }
    }
    }
    }
  wfst.segment_best(st,words,seg);
  seg.print(cerr,st);
*/
  
   
  string s;
  while (getline(cin,s)) {
    if (!s.empty()) {
      vector<string> ss;
      sentences_split(s,ss);
      //sentences.push_back(Sentence(s));
      //Sentence &st = sentences.back();
      for (int i = 0;i < ss.size();i ++) {
	//cout << ss[i] << endl;
	Sentence st(ss[i]);
	st.standardize();
	st.tokenize();
	if (!st.get_syllable_count())
		continue;
	Lattice words;
	words.construct(st);
	Segmentation seg(words.we);
	/*
	if (argc > 2)
	  wfst.segment_best_no_fuzzy(words,seg);
	else
	  wfst.segment_best(words,seg);
	*/
	Path path;
	WordDAG dagw(&words);
	DAG *dag = &dagw;
	if (threegram) {
	  WordDAG2 *dagw2 = new WordDAG2(&dagw);
	  dag = dagw2;
	}
	if (bellman) {
	  Bellman wfst;
	  wfst.search(*dag,path);
	} else {
	  PFS wfst;
	  wfst.search(*dag,path);
	}
	if (threegram) {
	  ((WordDAG2*)dag)->demangle(path);
	  delete (WordDAG2*)dag;
	}
	seg.resize(path.size()-2);
	copy(path.begin()+1,path.end()-1,seg.begin());
	seg.pretty_print(cout,st) << endl;
	//	sarch.clear_rest();
      }
    }
  }
    
  return 0;
}
