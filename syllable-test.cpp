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
    cout << Dictionary::first_consonants[i.first_consonant] << " ";
    cout << Dictionary::padding_vowels[i.padding_vowel] << " ";
    cout << Dictionary::vowels[i.vowel] << " "; 
    cout << Dictionary::last_consonants[i.last_consonant] << endl;
  }
  return 0;
}
