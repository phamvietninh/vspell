#include "wfst.h"
#include "distance.h"

void main()
{
  WFST wfst;
  WordNodePtr wl(new WordNode);

  ed_init();

  wl->load("wordlist.wl");
  wfst.set_wordlist(wl);
  WFST::Segmentation seg;
  Sentence st("h�c sinh h�c sinh h�c");
  st.standardize();
  st.tokenize();
  wfst.segment_best(st,seg);
}
