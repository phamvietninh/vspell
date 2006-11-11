#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <set>
#include "vspell.h"
#include "syllable.h"
#include "propername.h"

using namespace std;

void lattice_to_dot(ostream &os,const Lattice &w2,bool spare,bool edge_value)
{
  uint i,n;
  const Sentence &st = *w2.st;
  const WordEntries &wes = *w2.we;
  n = wes.size();
  if (n == 0) return;
  os << "digraph wordlattice {" << endl;
  os << "\trankdir=LR;" << endl;
  os << "\tstyle=invis;" << endl;
  os << "\thead;" << endl;
  os << "\ttail;" << endl;
  //set<strid> nodes;
  int old_pos = -1;
  int cc;
  int anchor[st.get_syllable_count()];
  for (i = 0;i < n;i ++) {
    //if (nodes.find(wes[i].node.node->get_id()) == nodes.end()) {
    //nodes.insert(wes[i].node.node->get_id());
    if (wes[i].pos != old_pos) {
      if (wes[i].pos) {
	os << "\t}" << endl;
      }
      os << "\tsubgraph cluster_" << wes[i].pos << " {" << endl;
      old_pos = wes[i].pos;
      cc = 0;
    }

    if (spare && cc++ == w2.get_we(wes[i].pos).size()/2)
      //os << "\tanchor_" << wes[i].pos << " [shape=\"point\"];" << endl;
      anchor[wes[i].pos] = i;

    os << "\tn" << i << " [label=\"";
    std::vector<strid> syll;
    if (wes[i].node.node) {
      wes[i].node.node->get_syllables(syll);
      for (std::vector<strid>::size_type ii = 0;ii < syll.size();ii ++) {
	if (i)
	  os << " ";
	Syllable sy;
	if (sy.parse(get_ngram()[syll[ii]]))
	  os << sy.to_str();
	else
	  os << get_ngram()[syll[ii]];
      }
    } else
      os << "UNK";
    os << "\"];" << endl;

    //}
  }
  os << "\t}" << endl;	// end of the last cluster

  if (spare)
    for (i = 0;i < st.get_syllable_count()-1;i ++) {
      //os << "anchor_" << i << " -> anchor_" << (i+1) << " [style=invis, weight=10000];" << endl;
      os << "n" << anchor[i] << " -> n" << anchor[i+1] << " [style=invis, weight=10000];" << endl;
    }

  VocabIndex vi[2];
  vi[1] = 0;
  float val;
  int ii,nn;
  for (i = 0;i < n;i ++) {
    const WordEntry &we = wes[i];

    if (we.pos == 0) {
      if (edge_value) {
	vi[0] = get_id(START_ID);
	val = -get_ngram().wordProb(we.node.node->get_id(),vi);
	os << "\thead -> n" << we.id << " [ label=\"" << val << "\"];" << endl;
      } else
	os << "\thead -> n" << we.id << ";" << endl;
    }
    if (we.pos+we.len >= w2.get_word_count()) {
      if (edge_value) {
	vi[0] = we.node.node->get_id();
	val = -get_ngram().wordProb(get_id(STOP_ID),vi);
	os << "\tn" << we.id << " -> tail [ label=\"" << val << "\"];" << endl;
      } else
	os << "\tn" << we.id << " -> tail;" << endl;
    } else {
      if (spare)
	os << "\tn" << we.id << " -> n" << anchor[(we.pos+we.len)] << ";" << endl;
      else {
	const WordEntryRefs &wers = w2.get_we(we.pos+we.len);
	nn = wers.size();
	for (ii = 0;ii < nn; ii ++) {
	  if (edge_value) {
	    vi[0] = we.node.node->get_id();
	    val = -get_ngram().wordProb(wers[ii]->node.node->get_id(),vi);
	    os << "\tn" << we.id << " -> n" << wers[ii]->id << " [label=\"" << val << "\"];" << endl;
	  } else
	    os << "\tn" << we.id << " -> n" << wers[ii]->id << ";" << endl;
	}
      }
    }
  }

  os << "}" << endl;
}

int main(int argc,char **argv)
{
  bool spare = false;
  bool edge_value = false;

  for (int i = 1;i < argc;i ++) {
    if (!strcmp(argv[i],"--spare")) {
      spare = true;
      continue;
    }
    if (!strcmp(argv[i],"--edge")) {
      if (i+1 == argc) {
        cerr << "ngram is needed for --edge" << endl;
	return -1;
      }
      if (!get_ngram().read(argv[i+1])) {
        cerr << "ngram " << argv[i+1] << " failed to load" << endl;
	return -2;
      }
      edge_value = true;
      i++;
      continue;
    }
    cerr << "Unknown parameter " << argv[i] << endl;
  }

  dic_init();
  warch.load("wordlist");

  int count = 0;
  while (!cin.eof()) {
    if (++count % 200 == 0) cerr << count << endl;
    Lattice l;
    cin >> l;
    lattice_to_dot(cout,l,spare,edge_value);
  }
  return 0;
}
