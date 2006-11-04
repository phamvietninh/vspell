#include <iostream>
#include <string>
#include "dictionary.h"
#include "syllable.h"

using namespace std;

bool syllable_init();
int main()
{
  Syllable i;
  string str;
  
  syllable_init();

  while (cin >> str) {
    cout << i.parse(str.c_str()) << ": ";
    cout << i << " " << i.to_str() << endl;
  }
  return 0;
}
