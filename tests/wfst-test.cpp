// -*- coding: viscii -*-
#include "wfst.h"
#include "distance.h"
#include "sentence.h"
#include <string>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <../libvspell/boost/format.hpp>

using namespace std;

void print_all_words(const Words &words);
int main(int argc,char **argv)
{
  WFST wfst;
  dic_init(argc > 1 ? new WordNode(sarch["<root>"]) : new FuzzyWordNode(sarch["<root>"]));
  ed_init();

  cerr << "Loading... ";
  get_root()->load("wordlist.wl");
  File f("ngram","rt");
  ngram.read(f);
  cerr << "done" << endl;

  sarch.set_blocked(true);

  sarch.dump();

  /*
    cerr << "Saving...";
    get_root()->save("wordlist2.wl");
    cerr << "done" << endl;
  */

  wfst.set_wordlist(get_root());
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
	cout << ss[i] << endl;
	Sentence st(ss[i]);
	st.standardize();
	st.tokenize();
	Segmentation seg;
	Words words;
	words.construct(st);
	wfst.segment_best(st,words,seg);
	cout << seg << endl;
	//	sarch.clear_rest();
      }
    }
  }
    
  return 0;
}
