#include "tokenize.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <spell.h>
#include <dictionary.h>

using namespace std;

int main()
{
  string str;

  dic_init(new WordNode(sarch["<root>"]));

  while (getline(cin,str)) {
    /*
    Tokens output;
    tokenize(str,output);
    //copy(output.begin(),output.end(),ostream_iterator<string>(cout,"\n"));
    for (int i = 0;i < output.size();i ++)
      if (output[i].is_token)
	cout << output[i].value << endl;
    */
    Sentence st(str);
    st.standardize();
    st.tokenize();
    cout << st << endl;
  }
  return 0;
}

