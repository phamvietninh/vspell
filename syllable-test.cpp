#include <iostream>
#include <string>
#include "dictionary.h"

using namespace std;

int main()
{
  Dictionary::Syllable i;
  string str;
  
  while (cin >> str) {
    cout << i.parse(str.c_str()) << ": ";
    i.print();
  }
  return 0;
}
