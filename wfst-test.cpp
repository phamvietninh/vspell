#include "wfst.h"
#include "distance.h"
#include <string>

void main()
{
  WFST wfst;
  Dictionary::initialize();
  ed_init();

  cerr << "Loading... ";
  Dictionary::get_root()->load("wordlist.wl");
  cerr << "done" << endl;

  wfst.set_wordlist(Dictionary::get_root());
  Segmentation seg;
  Sentence st("häc sinh däc sinh häc");
  st.standardize();
  st.tokenize();
  wfst.segment_best(st,seg);
}
