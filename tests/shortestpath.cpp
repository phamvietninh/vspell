#include "spell.h"
#include <iostream>
#include "sentence.h"

using namespace std;

class HeapValue : public vector<float> {
public:
  bool operator() (int v1,int v2) {
    return (*this)[v1] > (*this)[v2];
  }
  
};

void find_path(const Lattice &w)
{
  vector<int> back_traces;
  vector<bool> seen;
  vector<int> candidates;
  HeapValue val;

  int i,n = w.get_word_count();

  val.resize(n+1);
  back_traces.resize(n+1);
  seen.resize(n+1);
  candidates.reserve(n+1);

  candidates.push_back(0);
  seen[0] = true;
  val[0] = 0;

  int c = 1;

  while (c < n) {
    pop_heap(candidates.begin(),candidates.end(),val);
    int v = candidates.back();
    candidates.pop_back();
    cerr << "got " << v << " " << val[v] << endl;

    const WordEntryRefs &wers = w.get_we(v);
    int vv,ii,nn = wers.size();
    float value;
    for (ii = 0;ii < nn;ii ++) {
      vv = wers[ii]->pos+wers[ii]->len;
      value = val[v]+wers[ii]->node.node->get_prob();
      cerr << "examine " << vv << "(" << wers[ii]->node << ")";
      if (!seen[vv]) {
	candidates.push_back(vv);
	seen[vv] = true;
	val[vv] = value;
	push_heap(candidates.begin(),candidates.end(),val);
	back_traces[vv] = wers[ii]->id;
	cerr << " add " << val[vv] << "=" << v << "+"<<wers[ii]->node.node->get_prob();
	c++;
	cerr << " add";
      } else {
	if (val[vv] > value) {
	  val[vv] = value;
	  vector<int>::iterator iter = find(candidates.begin(),candidates.end(),vv);
	  while (true) {
	    push_heap(candidates.begin(),iter);
	    if (iter != candidates.end())
	      ++iter;
	    else
	      break;
	  }
	  back_traces[vv] = wers[ii]->id;
	  cerr << " val " << val[vv] << "=" << v << "+"<<wers[ii]->node.node->get_prob();
	}
      }
      cerr << endl;
    }
  }

  int p = n;
  vector<int> P;
  const WordEntries &we = *w.we;
  while (p) {
    int l = we[back_traces[p]].len;
    P.push_back(back_traces[p]);
    p -= l;
  }
  for (i = P.size()-1;i >= 0;i --) {
    cerr << we[P[i]] << endl;
  }

  cerr << "done" << endl;
}

int main(int argc,char **argv)
{
  dic_init(argc > 1 ? new WordNode(get_sarch()["<root>"]) : new FuzzyWordNode(get_sarch()["<root>"]));

  cerr << "Loading... ";
  get_root()->load("wordlist.wl");
  cerr << "done" << endl;

  get_sarch().set_blocked(true);

  string s;
  int count = 0;
  while (getline(cin,s)) {
    count ++;
    if (count % 200 == 0) cerr << count << endl;
    if (s.empty()) continue;
    vector<string> ss;
    sentences_split(s,ss);
    for (int i = 0;i < ss.size();i ++) {
      Sentence st(ss[i]);
      st.standardize();
      st.tokenize();
      Lattice words;
      words.construct(st);
      find_path(words);
    }
  }
}
