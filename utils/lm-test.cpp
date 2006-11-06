#include "dictionary.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
  dic_init();

  string s;
  vector<VocabIndex> v;
  while (cin >> s) {
    v.push_back(get_ngram()[s]);
  }
  for (int i = 0;i < v.size(); i ++) {
    if (i) cout << " ";
    cout << get_ngram()[v[i]];
  }
  cout << endl;
  return 0;
}
