#include "keyboard.h"
#include <string>

using namespace std;

int main(int argc,char **argv)
{
  string s;
  set<string> ss;
  while (cin >> s) {
    keyboard_recover(s.c_str(),ss);
  }
  return 0;
}
