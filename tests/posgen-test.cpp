#include "posgen.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;

int main()
{
  Generator gen;
  string s;
  while (getline(cin,s)) {
    if (s.empty()) continue;

    Sentence st(s);
    st.standardize();
    st.tokenize();
    gen.init(st);
    vector<int> pos;
    int len;
    while (gen.step(pos,len)) {
      for (int i = 0;i < len;i ++)
	cout << pos[i] << " ";
      cout << endl;
    }
  }
    
  return 0;
}
