#include "dictionary.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
  Dictionary::initialize();

  cerr << "Loading... ";
  Dictionary::get_root()->load("wordlist.wl");
  cerr << "done" << endl;

  string str;
  while (cin >> str) {
    cout << str << " -> " << Dictionary::is_syllable_exist(str) << endl;
  }
  return 0;
}
