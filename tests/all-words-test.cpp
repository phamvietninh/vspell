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

void lattice_to_dot(ostream &os,Lattice &w2,bool spare,bool has_seps,bool edge_value)
{
  uint i,n;
  const Sentence &st = *w2.st;
  WordEntries &wes = *w2.we;
  n = wes.size();
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
	if (sy.parse(get_sarch()[syll[ii]]))
	  os << sy.to_str();
	else
	  os << get_sarch()[syll[ii]];
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
  vi[1] = Vocab_None;
  float val;
  int ii,nn;
  for (i = 0;i < n;i ++) {
    WordEntry &we = wes[i];

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

void total_combinations(ostream &os,Lattice &w)
{
  WordEntries &wes = *w.we;
  unsigned long long nn = wes.size();
  vector<unsigned long long> val(nn);

  vector<vector<uint> > prev;
  int i,n = w.get_word_count(),v,vv;
  float sum = 0;

  prev.resize(nn);

  for (i = 0;i < n;i ++) {
    const WordEntryRefs &wers = w.get_we(i);
    int ii,nn = wers.size();
    for (ii = 0;ii < nn;ii ++) {
      // wers[ii] is the first node (W).
      v = wers[ii]->id;
      if (i == 0)
	val[v] = 1;
      int next = wers[ii]->pos+wers[ii]->len;
      if (next < n) {
	const WordEntryRefs &wers2 = w.get_we(next);
	int iii,nnn = wers2.size();
	for (iii = 0;iii < nnn;iii ++) {
	  //wers2[iii] is the second node (W).
	  vv = wers2[iii]->id;
	  prev[vv].push_back(v);
	}
      }
    }
  }

  unsigned long long final_val = 0;
  for (i = 0;i < n;i ++) {
    const WordEntryRefs &wers = w.get_we(i);
    int ii,nn = wers.size();
    for (ii = 0;ii < nn;ii ++) {
      // wers[ii] is the first node (W).
      v = wers[ii]->id;
      int iii,nnn = prev[v].size();
      for (iii = 0;iii < nnn;iii ++) {
	os << v << "(" << val[v] << ") <- "  << prev[v][iii] << "(" << val[prev[v][iii]] << ")" << endl;
	val[v] += val[prev[v][iii]];
      }
      if (wers[ii]->pos+wers[ii]->len == w.get_word_count()) {
	final_val += val[v];
	os << "Final: " << final_val << endl;
      }
    }
  }
  cout << final_val << endl;
}

int main(int argc,char **argv)
{
  WFST wfst;
  bool fuzzy = true;
  bool dot = false;
  bool spare = false;
  bool has_seps = false;
  bool edge_value = false;
  bool total_comb = false;

  int i,n;
  vector<unsigned> seps;

  for (i = 1;i < argc;i ++) {
    if (!strcmp(argv[i],"nofuzzy")) fuzzy = false;
    if (!strcmp(argv[i],"dot")) dot = true;
    if (!strcmp(argv[i],"spare")) spare = true;
    if (!strcmp(argv[i],"seps")) has_seps = true;
    if (!strcmp(argv[i],"edgeval")) edge_value = true;
    if (!strcmp(argv[i],"total_comb")) total_comb = true;
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
    WordStateFactories factories;
    ExactWordStateFactory exact;
    LowerWordStateFactory lower;
    FuzzyWordStateFactory fuzzy;
    factories.push_back(&exact);
    factories.push_back(&lower);
    factories.push_back(&fuzzy);
    w2.pre_construct(st,wes,factories);
    mark_proper_name(st,wes);
    if (has_seps)
      apply_separators(st,wes,seps);
    w2.post_construct(wes);
    //w2.based_on(words);
    if (total_comb)
      total_combinations(cout,w2);
    else {
      if (!dot)
	cout << w2;
      else 
	lattice_to_dot(cout,w2,spare,has_seps,edge_value);
    }
    get_sarch().clear_rest();
  }
    
  return 0;
}

