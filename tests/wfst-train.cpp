// -*- coding: viscii -*-
#include "wfst.h"
#include "distance.h"
#include <string>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <iostream>
#include "sentence.h"
#include <libsrilm/NgramStats.h>

using namespace std;

void iterate(ostream &os,int level);

WFST wfst;
vector<Sentence> sentences;

int main(int argc,char **argv)
{
  dic_init(argc > 1 ? new WordNode(sarch["<root>"]) : new WordNode(sarch["<root>"]));

  cerr << "Loading... ";
  get_root()->load("wordlist.wl");
  cerr << "done" << endl;

  wfst.set_wordlist(get_root());

  string s;
  while (getline(cin,s)) {
    if (s.empty())
      continue;
    vector<string> ss;
    sentences_split(s,ss);
    for (int i = 0;i < ss.size();i ++) {
      sentences.push_back(Sentence(ss[i]));
      Sentence &st = sentences.back();
      st.standardize();
      st.tokenize();
    }
  }
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

void print_all_words(const Words &words);
void iterate(ostream &os,int level)
{
  int ist,nr_sentences = sentences.size();
  NgramStats stats(sarch.get_dict(),2);
  for (ist = 0;ist < nr_sentences;ist ++) {
    Sentence &st = sentences[ist];
  
    Words words;
    words.construct(st);
    Segmentation seg(words.we);
    //print_all_words(words);
    wfst.segment_best(words,seg);
    cerr << seg << endl;


#ifdef TRAINING
    int i,ii,iii,n,nn,nnn;
    n = seg.items.size();
    VocabIndex *vi = new VocabIndex[n+1];
    vi[n] = Vocab_None;
    for (i = 0;i < n;i ++)
      vi[i] = seg.items[i].state->get_id();
    stats.countSentence(vi);

    n = words.size();
    for (i = 0;i < n;i ++) {
      nn = words[i].size();
      for (ii = 0;ii < nn;ii ++) {
	nnn = words[i][ii].fuzzy_match.size();
	for (iii = 0;iii < nnn;iii ++) {
	  words[i][ii].fuzzy_match[iii].node->inc_b();
	}
      }
    }

    n = seg.items.size();
    for (i = 0;i < n;i ++)
      seg.items[i].state->inc_a();
#endif
  }

  cerr << "Calculating... ";
  get_root()->get_next(unk_id)->get_b() = 0;
  get_root()->recalculate();
  ngram.estimate(stats);
  //wfst.enable_ngram(true);

  cerr << "Saving... ";
  ostringstream oss;
  oss << "wordlist.wl." << level;
  get_root()->save(oss.str().c_str());
  
  ostringstream oss1;
  oss1 << "ngram." << level;
  File f(oss1.str().c_str(),"wt");
  ngram.write(f);
}
