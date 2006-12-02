#include "tokenize.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <spell.h>
#include <dictionary.h>

using namespace std;

int main(int argc, char **argv)
{
  bool revert = argc == 2 && string(argv[1]) == "-r";
  string str;

  dic_init();

  int count = 0;
  get_ngram().set_blocked(true);
  while (getline(cin,str)) {
    if (++count % 200 == 0) cerr << count << endl;
    if (revert) {
      Sentence st(cin);
      cout << st << endl;
    }
    else {
      Sentence st(str);
      st.standardize();
      st.tokenize();
      cout << st << endl;
    }
    get_ngram().clear_oov();
  }
  return 0;
}
