#include "wfst.h"
#include "distance.h"
#include <string>

void main()
{
  WFST wfst;
  std::string root("-root-");
  WordNodePtr wl(new WordNode(root));

  ed_init();

  cerr << "Loading... ";
  wl->load("wordlist.wl");
  cerr << "done" << endl;

  wfst.set_wordlist(wl);
  Segmentation seg;
  Sentence st("häc sinh däc sinh häc");
  st.standardize();
  st.tokenize();
  wfst.segment_best(st,seg);
}
