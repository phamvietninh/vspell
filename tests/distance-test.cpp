#include <iostream>
#include <string>
#include <values.h>
#include <stdio.h>
#include "distance.h"

int main()
{
  string s1,s2;
  int i,j;

  ed_init();

  while (cin >> s1 >> s2) {
    int ret = ed(s1.data(),s1.size(),s2.data(),s2.size());
    ed_dump(s1.size(),s2.size());
    cout << ret << endl;
  }
}
