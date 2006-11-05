#include "wfst.h"
#include <fstream>
#include "sentence.h"

using namespace std;

int main(int argc,char **argv)
{
  WFST wfst;
  dic_init(argc > 1 ? new WordNode(get_sarch()["<root>"]) : new FuzzyWordNode(get_sarch()["<root>"]));

  cerr << "Loading... ";
  get_root()->load("wordlist.wl");
  cerr << "done" << endl;

  get_sarch().set_blocked(true);

  wfst.set_wordlist(get_root());

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
      //Lattice w;
      //w.based_on(words);
      //cerr << words << endl;
      Sections sects1,sects2;
      sects1.construct(words);
      //sects2.construct(w);
      //cout << sects1 << endl;
      //cout << sects2 << endl;
      //get_sarch().clear_rest();
    }
  }
    
  return 0;
}