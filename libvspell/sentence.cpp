#include "sentence.h"
using namespace std;

void sentences_split(const string &_input,vector<string> &output)
{
  // candidates are ? ! .
  // . is ambiguous
  string input = _input;

  int npos,pos = 0,len;
  bool run = true;
  bool split = false;

  while (run && !input.empty()) {
    if (split) {
      output.push_back(input.substr(0,npos+1));
      input.erase(0,npos+1);
      pos = 0;
      split = false;
    }

    npos = input.find_first_of("?!.",pos);
    if (npos == string::npos) break;

    len = input.size();

    if (!(npos + 1 < len)) break;
    if (input[npos+1] != ' ') continue;

    if (!(npos + 2 < len)) break;
    if (viet_isupper(input[npos+2])) { split = true; continue; }

    pos = npos+1;
  }

  if (!input.empty())
    output.push_back(input);
}
