// -*- coding: viscii -*-
#include "wfst.h"
#include "distance.h"
#include <string>
#include <fstream>
#include <cmath>
#include <cstdio>

using namespace std;

void print_all_words(const Words &words);
int main()
{
  WFST wfst;
  dic_init(new FuzzyWordNode(sarch["<root>"]));
  ed_init();

  cerr << "Loading... ";
  get_root()->load("wordlist.wl");
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
  ifstream ifs("corpus-test");
  if (!ifs.is_open()) {
    cerr << "Can not open corpus\n";
    return 0;
  }

/*
  Segmentation seg;
  Sentence st("tui v×a m¾i mua mµt chiªc ghe");
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
  while (getline(ifs,s)) {
    if (!s.empty()) {

      //sentences.push_back(Sentence(s));
      //Sentence &st = sentences.back();
      Sentence st(s);
      st.standardize();
      st.tokenize();
      Segmentation seg;
      Words words;
      wfst.get_all_words(st,words);
//      print_all_words(words);
      wfst.segment_best(st,words,seg);
      seg.print(cerr,st);
      sarch.clear_rest();
    }
  }
    
  return 0;
}
