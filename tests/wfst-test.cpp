// -*- coding: viscii -*-
#include "pfs.h"
#include "bellman.h"
#include "distance.h"
#include "sentence.h"
#include <string>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <../libvspell/boost/format.hpp>
#include "propername.h"

using namespace std;

void dag_to_dot(ostream &os,Lattice &w2,DAG &dag,bool edge_value);
void dag2_to_dot(ostream &os,Lattice &w2,WordDAG2 &dag,bool edge_value);
void print_all_words(const Lattice &words);

int main(int argc,char **argv)
{
  bool fuzzy = true;
  bool bellman = false;
  bool trigram = false;
  bool dot = false;
  bool edge_value = false;

  for (int i = 1;i < argc;i ++) {
    if (!strcmp(argv[i],"nofuzzy")) fuzzy = false;
    else if (!strcmp(argv[i],"bellman")) bellman = true;
    else if (!strcmp(argv[i],"trigram")) trigram = true;
    else if (!strcmp(argv[i],"dot")) dot = true;
    else if (!strcmp(argv[i],"edge")) edge_value = true;
  }
  dic_init();
  ed_init();

  cerr << "Loading... ";
  warch.load("wordlist");
  File f("ngram","rt",0);
  if (!f.error())
	  get_ngram().read(f);
  cerr << "done" << endl;

  get_sarch().set_blocked(true);

  get_sarch().dump();

  /*
    cerr << "Saving...";
    get_root()->save("wordlist2.wl");
    cerr << "done" << endl;
  */

  //wfst.set_wordlist(get_root());
  vector<Sentence> sentences;

  string s;
  while (getline(cin,s)) {
    if (!s.empty()) {
      vector<string> ss;
      sentences_split(s,ss);
      //sentences.push_back(Sentence(s));
      //Sentence &st = sentences.back();
      for (int i = 0;i < ss.size();i ++) {
	//cout << ss[i] << endl;
	Sentence st(ss[i]);
	st.standardize();
	st.tokenize();
	if (!st.get_syllable_count())
		continue;
	Lattice words;
	set<WordEntry> wes;
	WordStateFactories factories;
	ExactWordStateFactory exact;
	LowerWordStateFactory lower;
	UpperWordStateFactory upper;
	FuzzyWordStateFactory ffuzzy;
	factories.push_back(&exact);
	factories.push_back(&lower);
	if (fuzzy) {
	  factories.push_back(&upper);
	  factories.push_back(&ffuzzy);
	}
	words.pre_construct(st,wes,factories);
	mark_proper_name(st,wes);
	words.post_construct(wes);
	Segmentation seg(words.we);
	/*
	if (argc > 2)
	  wfst.segment_best_no_fuzzy(words,seg);
	else
	  wfst.segment_best(words,seg);
	*/
	Path path;
	WordDAG dagw(&words);
	DAG *dag = &dagw;
	WordDAG2 *dagw2;
	if (trigram) {
	  dagw2 = new WordDAG2(&dagw);
	  dag = dagw2;
	}
	if (dot) {
	  if (trigram)
	    dag2_to_dot(cout,words,*dagw2,edge_value);
	  else
	    dag_to_dot(cout,words,*dag,edge_value);
	} else {
	  if (bellman) {
	    Bellman wfst;
	    wfst.search(*dag,path);
	  } else {
	    PFS wfst;
	    wfst.search(*dag,path);
	  }
	  if (trigram) {
	    ((WordDAG2*)dag)->demangle(path);
	    delete (WordDAG2*)dag;
	  }
	  seg.resize(path.size()-2);
	  copy(path.begin()+1,path.end()-1,seg.begin());
	  seg.pretty_print(cout,st) << endl;
	}
	//	sarch.clear_rest();
      }
    }
  }
    
  return 0;
}

void dag_to_dot(ostream &os,Lattice &w2,DAG &dag,bool edge_value)
{
  uint i,n;
  const Sentence &st = *w2.st;
  WordEntries &wes = *w2.we;
  n = dag.node_count();
  os << "digraph wordlattice {" << endl;
  os << "\trankdir=LR;" << endl;
  os << "\tstyle=invis;" << endl;

  for (i = 0;i < n;i ++) {
    string label;
    label = get_sarch()[dag.node_id(i)];
      /*
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
      */
    os << "\tn" << i << "[label=\"" << label << ";" << i << "\"];" << endl;
    
  }

  vector<uint> nexts;
  vector<bool> done;
  uint v,vv,l,ii,nn;
  i = 0;
  done.resize(dag.node_count());
  nexts.clear();
  nexts.push_back(dag.node_begin());
  while (i < (l = nexts.size())) {
    v = nexts[i++];
    if (done[v])
      continue;
    else
      done[v] = true;
    dag.get_next(v,nexts);
    nn = nexts.size();
    for (ii = l;ii < nn;ii ++) {
      if (edge_value)
	os << "\tn" << v << " -> n" << nexts[ii] << "[label=\"" << dag.edge_value(v,nexts[ii]) << "\"];" << endl;
      else
	os << "\tn" << v << " -> n" << nexts[ii] << ";" << endl;
    }
  }
  done.clear();

  os << "}" << endl;
}

void dag2_to_dot(ostream &os,Lattice &w2,WordDAG2 &dag,bool edge_value)
{
  uint i,n;
  const Sentence &st = *w2.st;
  WordEntries &wes = *w2.we;
  n = dag.node_count();
  WordDAG *wdag = dag.get_dag();
  os << "digraph wordlattice {" << endl;
  os << "\trankdir=LR;" << endl;
  os << "\tstyle=invis;" << endl;

  for (i = 0;i < n;i ++) {
    string label;
    uint n1,n2;
    if (i == dag.node_begin() || i == dag.node_end())
      label = get_sarch()[dag.node_id(i)];
    else {
      dag.node_dag_edge(i,n1,n2);
      label = get_sarch()[wdag->node_id(n1)];
      label += string(" ");
      label += get_sarch()[wdag->node_id(n2)];
    }
    os << "\tn" << i << "[label=\"" << label << ";" << i << "\"];" << endl;
  }

  vector<uint> nexts;
  vector<bool> done;
  uint v,vv,l,ii,nn;
  i = 0;
  done.resize(dag.node_count());
  nexts.clear();
  nexts.push_back(dag.node_begin());
  while (i < (l = nexts.size())) {
    v = nexts[i++];
    if (done[v])
      continue;
    else
      done[v] = true;
    dag.get_next(v,nexts);
    nn = nexts.size();
    for (ii = l;ii < nn;ii ++) {
      if (edge_value)
	os << "\tn" << v << " -> n" << nexts[ii] << "[label=\"" << dag.edge_value(v,nexts[ii]) << "\"];" << endl;
      else
	os << "\tn" << v << " -> n" << nexts[ii] << ";" << endl;
    }
  }
  done.clear();

  os << "}" << endl;
}



