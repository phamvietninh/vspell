#include "wfst.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <set>
#include "propername.h"

using namespace std;

void apply_separators(const Sentence &st,set<WordEntry> &wes,vector<unsigned> &seps)
{
  sort(seps.begin(),seps.end());
  //copy(seps1.begin(),seps1.end(),inserter(seps,seps.begin()));
  int sep = 0,offset=0;
  int i,n = st.get_syllable_count();

  for (i = 0;i < n-1 && sep < seps.size();i ++) {
    int p = offset+st[i].start+strlen(get_sarch()[st[i].get_id()]);
    if (p <= seps[sep] && seps[sep] <= offset+st[i+1].start) {
      apply_separator(wes,i);
      sep ++;
    }
  }
}

int main(int argc,char **argv)
{
  WFST wfst;
  bool fuzzy = true;
  bool dot = false;
  bool spare = false;
  bool has_seps = false;
  bool edge_value = false;
  int i,n;
  vector<unsigned> seps;

  for (i = 1;i < argc;i ++) {
    if (!strcmp(argv[i],"nofuzzy")) fuzzy = false;
    if (!strcmp(argv[i],"dot")) dot = true;
    if (!strcmp(argv[i],"spare")) spare = true;
    if (!strcmp(argv[i],"seps")) has_seps = true;
    if (!strcmp(argv[i],"edgeval")) edge_value = true;
  }

  dic_init(fuzzy ?
	   new FuzzyWordNode(get_sarch()["<root>"]) :
	   new WordNode(get_sarch()["<root>"]));

  cerr << "Loading... ";
  get_root()->load("wordlist");
  File f("ngram","rt",0);
  if (!f.error())
	  get_ngram().read(f);
  cerr << "done" << endl;

  get_sarch().set_blocked(true);

  wfst.set_wordlist(get_root());

  string s;
  while (getline(cin,s)) {
    if (s.empty()) continue;

    if (has_seps) {
      string::size_type p;
      while ((p = s.find('|')) != string::npos) {
	seps.push_back(p);
	s.erase(p,1);
      }
    }

    Sentence st(s);
    st.standardize();
    st.tokenize();
    Lattice words,w2;
    set<WordEntry> wes;
    w2.pre_construct(st,wes);
    mark_proper_name(st,wes);
    if (has_seps)
      apply_separators(st,wes,seps);
    w2.post_construct(wes);
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
	    if (sy.parse(get_sarch()[syll[ii]]))
	      cout << sy.to_str();
	    else
	      cout << get_sarch()[syll[ii]];
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

      VocabIndex vi[2];
      vi[1] = Vocab_None;
      float val;
      int ii,nn;
      for (i = 0;i < n;i ++) {
	WordEntry &we = wes[i];

	if (we.pos == 0) {
	  if (edge_value) {
	    vi[0] = get_id(START_ID);
	    val = -get_ngram().wordProb(we.node.node->get_id(),vi);
	    cout << "\thead -> n" << we.id << " [ label=\"" << val << "\"];" << endl;
	  } else
	    cout << "\thead -> n" << we.id << ";" << endl;
	}
	if (we.pos+we.len >= w2.get_word_count()) {
	  if (edge_value) {
	    vi[0] = we.node.node->get_id();
	    val = -get_ngram().wordProb(get_id(STOP_ID),vi);
	    cout << "\tn" << we.id << " -> tail [ label=\"" << val << "\"];" << endl;
	  } else
	    cout << "\tn" << we.id << " -> tail;" << endl;
	} else {
	  if (!spare)
	    cout << "\tn" << we.id << " -> n" << anchor[(we.pos+we.len)] << ";" << endl;
	  else {
	    const WordEntryRefs &wers = w2.get_we(we.pos+we.len);
	    nn = wers.size();
	    for (ii = 0;ii < nn; ii ++) {
	      if (edge_value) {
		vi[0] = we.node.node->get_id();
		val = -get_ngram().wordProb(wers[ii]->node.node->get_id(),vi);
		cout << "\tn" << we.id << " -> n" << wers[ii]->id << " [label=\"" << val << "\"];" << endl;
	      } else
		cout << "\tn" << we.id << " -> n" << wers[ii]->id << ";" << endl;
	    }
	  }
	}
      }

      cout << "}" << endl;
    }
    get_sarch().clear_rest();
  }
    
  return 0;
}
