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
  Sentence st("h�c sinh d�c sinh h�c");
  st.standardize();
  st.tokenize();
  wfst.segment_best(st,seg);
}
