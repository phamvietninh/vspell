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
  bool spare = false;
  int i,n;

  for (i = 1;i < argc;i ++) {
    if (!strcmp(argv[i],"nofuzzy")) fuzzy = false;
    if (!strcmp(argv[i],"dot")) dot = true;
    if (!strcmp(argv[i],"spare")) spare = true;
  }

  dic_init(fuzzy ? new FuzzyWordNode(sarch["<root>"]) : new WordNode(sarch["<root>"]));

  cerr << "Loading... ";
  get_root()->load("wordlist");
  cerr << "done" << endl;

  sarch.set_blocked(true);

  wfst.set_wordlist(get_root());

  string s;
  while (getline(cin,s)) {
    if (s.empty()) continue;

    Sentence st(s);
    st.standardize();
    st.tokenize();
    Lattice words,w2;
    w2.construct(st);
    //w2.based_on(words);
    if (!dot)
      cout << w2;
    else {
      WordEntries &wes = *w2.we;
      n = wes.size();
      cout << "digraph wordlattice {" << endl;
      cout << "\trankdir=LR;" << endl;
      cout << "\tstyle=invis;" << endl;
      cout << "\thead;" << endl;
      cout << "\ttail;" << endl;
      //set<strid> nodes;
      int old_pos = -1;
      int cc;
      int anchor[st.get_syllable_count()];
      for (i = 0;i < n;i ++) {
	//if (nodes.find(wes[i].node.node->get_id()) == nodes.end()) {
	//nodes.insert(wes[i].node.node->get_id());
	if (wes[i].pos != old_pos) {
	  if (wes[i].pos) {
	    cout << "\t}" << endl;
	  }
	  cout << "\tsubgraph cluster_" << wes[i].pos << " {" << endl;
	  old_pos = wes[i].pos;
	  cc = 0;
	}

	if (!spare && cc++ == w2.get_we(wes[i].pos).size()/2)
	  //cout << "\tanchor_" << wes[i].pos << " [shape=\"point\"];" << endl;
	  anchor[wes[i].pos] = i; 

	cout << "\tn" << i << " [label=\"";
	std::vector<strid> syll;
	if (wes[i].node.node) {
	  wes[i].node.node->get_syllables(syll);
	  for (std::vector<strid>::size_type ii = 0;ii < syll.size();ii ++) {
	    if (i)
	      cout << " ";
	    Syllable sy;
	    if (sy.parse(sarch[syll[ii]]))
	      cout << sy.to_str();
	    else
	      cout << sarch[syll[ii]];
	  }
	} else
	  cout << "UNK";
	cout << "\"];" << endl;

	//}
      }
      cout << "\t}" << endl;	// end of the last cluster

      if (!spare)
	for (i = 0;i < st.get_syllable_count()-1;i ++) {
      	  //cout << "anchor_" << i << " -> anchor_" << (i+1) << " [style=invis, weight=10000];" << endl;
      	  cout << "n" << anchor[i] << " -> n" << anchor[i+1] << " [style=invis, weight=10000];" << endl;
	}

      int ii,nn;
      for (i = 0;i < n;i ++) {
	WordEntry &we = wes[i];

	if (we.pos == 0)
	  cout << "\thead -> n" << we.id << ";" << endl;
	if (we.pos+we.len >= w2.get_word_count())
	  cout << "\tn" << we.id << " -> tail;" << endl;
	else {
	  if (!spare)
	    cout << "\tn" << we.id << " -> n" << anchor[(we.pos+we.len)] << ";" << endl;
	  else {
	    const WordEntryRefs &wers = w2.get_we(we.pos+we.len);
	    nn = wers.size();
	    for (ii = 0;ii < nn; ii ++) {
	      cout << "\tn" << we.id << " -> n" << wers[ii]->id << ";" << endl;
	    }
	  }
	}
      }

      cout << "}" << endl;
    }
    sarch.clear_rest();
  }
    
  return 0;
}
