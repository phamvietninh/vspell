#include <syllable.h>
#include <iostream>
#include <string>
#include <wordnode.h>

using namespace std;

int main(int argc,char **argv)
{
  dic_init();
  string s;
  while (cin >> s) {
    cout << get_std_syllable(s) << endl;
  }
  return 0;
}
