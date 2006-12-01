#include <syllable.h>
#include <iostream>
#include <string>
#include <wordnode.h>
#include <sstream>

using namespace std;

int main(int argc,char **argv)
{
  dic_init();
  string s;
  while (getline(cin,s)) {
    istringstream iss(s);
    string ss;
    bool first = true;
    while (iss >> ss) {
      if (first)
        first = false;
      else
        cout << " ";
      cout << get_std_syllable(ss);
    }
    cout << endl;
  }
  return 0;
}
