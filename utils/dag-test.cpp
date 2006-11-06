#include <algorithm>
#include <iostream>
#include <iterator>
#include <spell.h>
#include <dag.h>
#include <dictionary.h>

using namespace std;

int main(int argc, char **argv)
{
  get_ngram().read("ngram");
  dic_init();
  warch.load("wordlist");

  int count = 0;
  while (!cin.eof()) {
    if (++count % 200 == 0) cerr << count << endl;
    Lattice l;
    cin >> l;
    WordDAG dag(&l);
    cout << dag << endl;
  }
  return 0;
}
