#include "sentence.h"
#include "dictionary.h"
#include "wordnode.h"
#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;

int main()
{
  string str;

  dic_init(new WordNode(sarch["<root>"]));
  
  while (getline(cin,str)) {
    vector<string> output;
    sentences_split(str,output);
    cout << "Result: \n";
    copy(output.begin(),output.end(),ostream_iterator<string>(cout,"\n-->"));
  }
  return 0;
}

