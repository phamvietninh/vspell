#include <algorithm>
#include <iostream>
#include <iterator>
#include <spell.h>
#include <dag.h>
#include <pfs.h>
#include <dictionary.h>

using namespace std;

int main(int argc, char **argv)
{
  get_ngram().read(argc == 2 ? argv[1] : "ngram");
  dic_init();
  warch.load("wordlist");

  int count = 0;
  while (!cin.eof()) {
    if (++count % 200 == 0) cerr << count << endl;
    Lattice l;
    cin >> l;
    WordDAG dag(&l);
    Path p;
    PFS pfs;
    pfs.search(dag,p);
    Segmentation seg;
    if (p.empty() || p.size() <= 2)
      continue;
    seg.resize(p.size()-2);
    copy(p.begin()+1,p.end()-1,seg.begin());
    seg.we = l.we;
    cout << seg << endl;
  }
  return 0;
}
