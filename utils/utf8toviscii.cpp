#include <iostream>
#include <string>
#include "vspell.h"
using namespace std;
extern void viet_init();
int main(int argc,char **argv)
{
	string s;

	viet_init();
	bool revert = argc > 1 && string(argv[1]) == "-r";

	if (revert) {
		while (getline(cin,s)) {
			char *ss = new char[s.size()*3+1];
			viet_viscii_to_utf8(s.c_str(),ss);
			cout << ss << endl;
			delete[] ss;
		}
	}
	else {
		while (getline(cin,s)) {
			char *ss = new char[s.size()+1];
			viet_utf8_to_viscii(s.c_str(),ss);
			cout << ss << endl;
			delete[] ss;
		}
	}
	return 0;
}
