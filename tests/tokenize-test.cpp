#include "tokenize.h"
#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;

int main()
{
  string str;

  while (getline(cin,str)) {
    Tokens output;
    tokenize(str,output);
    //copy(output.begin(),output.end(),ostream_iterator<string>(cout,"\n"));
    for (int i = 0;i < output.size();i ++)
      if (output[i].is_token)
	cout << output[i].value << endl;
  }
  return 0;
}

