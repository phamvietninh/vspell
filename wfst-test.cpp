// -*- coding: viscii -*-
#include "wfst.h"
#include "distance.h"
#include <string>
#include <fstream>
#include <cmath>
#include <cstdio>

int main()
{
  WFST wfst;
  Dictionary::initialize();
  ed_init();
  

  cerr << "Loading... ";
  Dictionary::get_root()->load("wordlist.wl");
  cerr << "done" << endl;

  /*
    cerr << "Saving...";
    Dictionary::get_root()->save("wordlist2.wl");
    cerr << "done" << endl;
  */

  wfst.set_wordlist(Dictionary::get_root());
  vector<Sentence> sentences;
  ifstream ifs("corpus3");
  if (!ifs.is_open()) {
    cerr << "Can not open corpus\n";
    return 0;
  }
  /*
    Sentence st("h÷c zinh h÷c sinh h÷c");
    st.standardize();
    st.tokenize();
    wfst.segment_best(st,seg);
  */
  

  string s;
  while (getline(ifs,s)) {
    if (!s.empty()) {
      sentences.push_back(Sentence(s));
      Sentence &st = sentences.back();
      st.standardize();
      st.tokenize();
      Segmentation seg;
      vector<WFST::WordInfos> words;
      wfst.get_all_words(st,words);
      wfst.segment_best(st,words,seg);

      /*
      {
	int nn = words.size();
	for (int i = 0;i < nn;i ++) {
	  int nnn = words[i].size();
	  cerr << "From " << i << endl;
	  for (int ii = 0;ii < nnn;ii ++) {
	    int nnnn = words[i][ii].fuzzy_match.size();
	    cerr << "Len " << ii << endl;
	    for (int iii = 0;iii < nnnn;iii ++) {
	      cerr << words[i][ii].fuzzy_match[iii].distance << " ";
	      cerr << words[i][ii].fuzzy_match[iii].node->get_prob() << endl;
	    }
	  }
	}
      }
      */

#ifdef TRAINING
      vector<int> v;
      v.reserve(4);

      int i,ii,iii,n,nn,nnn;
      n = words.size();
      for (i = 0;i < n;i ++) {
	nn = words[i].size();
	for (ii = 0;ii < nn;ii ++) {
	  nnn = words[i][ii].fuzzy_match.size();
	  for (iii = 0;iii < nnn;iii ++) {
	    v.clear();
	    words[i][ii].fuzzy_match[iii].node->b ++;
	  }
	}
      }

      n = seg.items.size();
      for (i = 0;i < n;i ++) {
	v.clear();
	seg.items[i].state->a ++;
      }
#endif
    }
  }

  return 0;
}
