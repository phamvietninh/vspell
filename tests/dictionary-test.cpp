#include "dictionary.h"
#include "wordnode.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
  dic_init(new WordNode::WordNode(get_sarch()["<-root->"]));

  cerr << "Loading... ";
  get_root()->load("wordlist.wl");
  cerr << "done" << endl;

  string str;
  while (cin >> str) {
    cout << str << " -> " << is_syllable_exist(str) << endl;
  }
  return 0;
}
