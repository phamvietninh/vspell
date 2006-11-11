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
  bool revert = argc >= 2 && string(argv[1]) == "-r";
  int revert_level = 0;
  if (argc == 3) {
    if (string(argv[2]) == "--sentence" || string(argv[2]) == "2")
      revert_level = 2;
    if (string(argv[2]) == "--token" || string(argv[2]) == "1")
      revert_level = 1;
  }
  dic_init();
  warch.load("wordlist");

  int count = 0;
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
      Sentence st(cin);
      WordStateFactories factories;
      ExactWordStateFactory exact;
      LowerWordStateFactory lower;
      //FuzzyWordStateFactory fuzzy;
      factories.push_back(&exact);
      factories.push_back(&lower);
      //factories.push_back(&fuzzy);
      words.pre_construct(st,wes,factories);
      mark_proper_name(st,wes);
      words.post_construct(wes);
      cout << words << endl;
    }
    //cerr << words << endl;
    /*
    WordDAG dagw(&words);
    DAG *dag = &dagw;
    WordDAG2 *dagw2;
    if (trigram) {
      dagw2 = new WordDAG2(&dagw);
      dag = dagw2;
    }
    */
  }
  return 0;
}
