#include "wfst.h"
#include "sentence.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;

int main(int argc,char **argv)
{
  dic_init();

  cerr << "Loading... ";
  warch.load("wordlist.wl");

  File f("ngram","rt");
  get_ngram().read(f);
  cerr << "done" << endl;

  get_sarch().set_blocked(true);

  string s;
  while (getline(cin,s)) {
    if (s.empty()) continue;

    vector<string> ss;
    sentences_split(s,ss);
    for (int ii = 0;ii < ss.size();ii ++) {
      Sentence st(ss[ii]);
      st.standardize();
      st.tokenize();
      Lattice words;
      Segmentation seg;
      Segmentor segtor;
      words.construct(st);
      segtor.init(words,0,4);
      while (segtor.step(seg)) {
	int ngram_length=2;
	seg.prob = 0;
	VocabIndex *vi = new VocabIndex[ngram_length];
	vi[ngram_length] = Vocab_None;
	for (int i = ngram_length-1;i < seg.size();i ++) {
	  for (int j = 0;j < ngram_length-1;j++)
	    vi[j] = seg[i-1-j].node.node->get_id();
	  seg.prob += -get_ngram().wordProb(seg[i].node.node->get_id(),vi);
	}
      
	cout << seg << " " << seg.prob << endl;
	//cout << seg << endl;
      }
      segtor.done();
    }
  }
    
  return 0;
}
