#include "sentence.h"
#include "dictionary.h"
#include <algorithm>

using namespace std;
using namespace Dictionary;

int main()
{
  string str;

  initialize(new WordNode(sarch["<root>"]));
  
  while (getline(cin,str)) {
    vector<string> output;
    sentences_split(str,output);
    cout << "Result: \n";
    copy(output.begin(),output.end(),ostream_iterator<string>(cout,"\n"));
  }
  return 0;
}

