// -*- coding: viscii -*-
#include "wfst.h"
#include "distance.h"
#include <string>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <srilm/NgramStats.h>

void iterate(ostream &os,int level);

using namespace std;

WFST wfst;
vector<Sentence> sentences;

int main()
{
  Dictionary::initialize();

  cerr << "Loading... ";
  Dictionary::get_root()->load("wordlist.wl");
  cerr << "done" << endl;

  wfst.set_wordlist(Dictionary::get_root());
  ifstream ifs("corpus2");
  if (!ifs.is_open()) {
    cerr << "Can not open corpus\n";
    return 0;
  }

  string s;
  while (getline(ifs,s)) {
    if (!s.empty()) {
      sentences.push_back(Sentence(s));
      Sentence &st = sentences.back();
      st.standardize();
      st.tokenize();
    }
  }

  for (int i = 0;i < 50;i ++) {
    ostringstream oss;
    oss << "log." << i;
    ofstream ofs(oss.str().c_str());
    cerr << "Iteration " << i << "... ";
    iterate(ofs,i);
    cerr << "done" << endl;
  }

  return 0;
}

void iterate(ostream &os,int level)
{
  int ist,nr_sentences = sentences.size();
  NgramStats stats(Dictionary::sarch.get_dict(),2);
  for (ist = 0;ist < nr_sentences;ist ++) {
    Sentence &st = sentences[ist];
  
    Segmentation seg;
    Words words;
    wfst.get_all_words(st,words);
    /*
    {
    int i, nn = words.size();
    for (i = 0;i < nn;i ++) {
    int nnn = words[i].size();
    cerr << "From " << i << endl;
    for (int ii = 0;ii < nnn;ii ++) {
    int nnnn = words[i][ii].fuzzy_match.size();
    cerr << "Len " << ii << endl;
    for (int iii = 0;iii < nnnn;iii ++) {
    cerr << Dictionary::sarch[words[i][ii].fuzzy_match[iii].node->get_id()] << " ";
    cerr << words[i][ii].fuzzy_match[iii].node->get_syllable_count() << " ";
    cerr << words[i][ii].fuzzy_match[iii].distance << " ";
    cerr << words[i][ii].fuzzy_match[iii].node->get_prob() << endl;
    }
    }
    }
    }*/
    wfst.segment_best(st,words,seg);
    seg.print(os,st);


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
  Dictionary::get_root()->get_next(Dictionary::unk_id)->get_b() = 0;
  Dictionary::get_root()->recalculate();
  Dictionary::ngram.estimate(stats);
  //wfst.enable_ngram(true);

  cerr << "Saving... ";
  ostringstream oss;
  oss << "wordlist.wl." << level;
  Dictionary::get_root()->save(oss.str().c_str());
  
  ostringstream oss1;
  oss1 << "ngram." << level;
  File f(oss1.str().c_str(),"wt");
  Dictionary::ngram.write(f);
}
