#include "wfst.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;

int main()
{
  WFST wfst;
  dic_init(new FuzzyWordNode(sarch["<root>"]));

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
    words.construct(st);
    w2.based_on(words);
    cout << w2;
    sarch.clear_rest();
  }
    
  return 0;
}
