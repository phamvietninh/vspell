#include "posgen.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;

int main()
{
  PosGen gen;
  string s;
  uint len,n;
  while (cin >> len >> n) {
    gen.init(len,n);
    vector<uint> pos;
    uint len;
    while (gen.step(pos,len)) {
      for (int i = 0;i < len;i ++)
	cout << pos[i] << " ";
      cout << endl;
    }
  }
    
  return 0;
}
