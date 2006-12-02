#include "propername.h"
#include "tokenize.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <spell.h>
#include <dictionary.h>

using namespace std;

int main(int argc, char **argv)
{
  bool revert = false;
  bool use_fuzzy = false;
  int revert_level = 0;
  for (int i = 1;i < argc;i ++) {
    if (string(argv[i]) == "-r")
      revert = true;
    if (string(argv[i]) == "--sentence" || string(argv[i]) == "2")
      revert_level = 2;
    if (string(argv[i]) == "--token" || string(argv[i]) == "1")
      revert_level = 1;
    if (string(argv[i]) == "--fuzzy")
      use_fuzzy = true;
  }
  dic_init();
  warch.load("wordlist");

  int count = 0;
  get_ngram().set_blocked(true);
  while (!cin.eof()) {
    if (++count % 200 == 0) cerr << count << endl;
    Lattice words;
    set<WordEntry> wes;
    if (revert) {
      cin >> words;
      switch (revert_level) {
      case 0: cout << words << endl; break;
      case 1: cout << *words.st << endl; break;
      case 2: cout << words.st->get() << endl; break;
      }
    }
    else {
      boost::shared_ptr<Sentence> st(new Sentence(cin));
      WordStateFactories factories;
      ExactWordStateFactory exact;
      LowerWordStateFactory lower;
      FuzzyWordStateFactory fuzzy;
      factories.push_back(&exact);
      factories.push_back(&lower);
      if (use_fuzzy)
        factories.push_back(&fuzzy);
      words.pre_construct(st,wes,factories);
      mark_proper_name(*st,wes);
      words.post_construct(wes);
      cout << words << endl;
    }
    get_ngram().clear_oov();
  }
  return 0;
}
