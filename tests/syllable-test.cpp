#include <iostream>
#include <string>
#include "dictionary.h"
#include "syllable.h"

using namespace std;

int main()
{
  Syllable i;
  string str;
  
  while (cin >> str) {
    cout << i.parse(str.c_str()) << ": ";
    cout << i << endl;
  }
  return 0;
}
