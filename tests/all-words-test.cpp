#include "wfst.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <set>

using namespace std;

int main(int argc,char **argv)
{
  WFST wfst;
  bool fuzzy = true;
  bool dot = false;
  int i,n;

  for (i = 1;i < argc;i ++) {
   if (!strcmp(argv[i],"nofuzzy")) fuzzy = false;
   if (!strcmp(argv[i],"dot")) dot = true;
  }

  dic_init(fuzzy ? new FuzzyWordNode(sarch["<root>"]) : new WordNode(sarch["<root>"]));

  cerr << "Loading... ";
  get_root()->load("wordlist.wl");
  cerr << "done" << endl;

  sarch.set_blocked(true);

  wfst.set_wordlist(get_root());

  string s;
  while (getline(cin,s)) {
    if (s.empty()) continue;

    Sentence st(s);
    st.standardize();
    st.tokenize();
    Words words,w2;
    w2.construct(st);
    //w2.based_on(words);
    if (!dot)
      cout << w2;
    else {
      WordEntries &wes = *w2.we;
      n = wes.size();
      cout << "digraph wordlattice {" << endl;
      cout << "\trankdir=LR;" << endl;
      cout << "\thead;" << endl;
      cout << "\ttail;" << endl;
      //set<strid> nodes;
      for (i = 0;i < n;i ++) {
	//if (nodes.find(wes[i].node.node->get_id()) == nodes.end()) {
	//nodes.insert(wes[i].node.node->get_id());
	  cout << "\tn" << i << " [label=\"";
	  std::vector<strid> syll;
	  wes[i].node.node->get_syllables(syll);
	  for (std::vector<strid>::size_type ii = 0;ii < syll.size();ii ++) {
	    if (i)
	      cout << " ";
	    cout << sarch[syll[ii]];
	  }
	  cout << "\"];" << endl;
	  //}
      }

      int ii,nn;
      for (i = 0;i < n;i ++) {
	WordEntry &we = wes[i];

	if (we.pos == 0)
	  cout << "\thead -> n" << we.id << ";" << endl;
	if (we.pos+we.len => w2.get_word_count())
	  cout << "\tn" << we.id << " -> tail;" << endl;
	else {
	  const WordEntryRefs &wers = w2.get_we(we.pos+we.len);
	  nn = wers.size();
	  for (ii = 0;ii < nn; ii ++) {
	    cout << "\tn" << we.id << " -> n" << wers[ii]->id << ";" << endl;
	  }
	}
      }

      cout << "}" << endl;
    }
    sarch.clear_rest();
  }
    
  return 0;
}
