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

  dic_init();
  
  int count = 0;
  while (getline(cin,str)) {
    if (++count % 200 == 0) cerr << count << endl;
    vector<string> output;
    sentences_split(str,output);
    copy(output.begin(),output.end(),ostream_iterator<string>(cout,"\n"));
  }
  return 0;
}

