// -*- coding: viscii tab-width: 2 mode: c++ -*-

#include "keyboard.h"

static char *keys = "`~1!2@3#4$5%6^7&8*9(0)-_=+\\|" "qwertyuiop[{]}" "asdfghjkl;:'\"" "zxcvbnm,<.>/?";
static char *keymap[] = {
	"~1!",												// `
	"`1!",												// ~
	"!`~q2@",											// 1
	"1`~q2@",											// !
	"@1!qw3#",										// 2
	"21!qw3#",										// @
	"#2@we4$",										// 3
	"32@we4$",										// #
	"$3#er5%",										// 4
	"43#er5%",										// $
	"%4$rt6^",										// 5
	"54$rt6^",										// %
	"^5%ty7&",										// 6
	"65%ty7&",										// ^
	"&6^yu8*",										// 7
	"76^yu8*",										// &
	"*7&ui9(",										// 8
	"87&ui9(",										// *
	"(8*io0)",										// 9
	"98*io0)",										// (
	")9(op-_",										// 0
	"09(op-_",										// )
	"_0)p[=+",										// -
	"-0)p[=+",										// _
	"+-_[{]}\\|",									// =
	"=-_[{]}\\|",									// +
	"|=+]}",											//
	"\\=+]}",											// |

	"aw2@1!",											// q
	"qase3#2@",										// w
	"wsdr4$3#",										// e
	"edft5%4$",										// r
	"rfgy6^5%",										// t
	"tghu7&6^",										// y
	"yhji8*7&",										// u
	"ujko9(8*",										// i
	"iklp0)9(",										// o
	"ol;:[{-_0)",									// p
	"{p;:'\"]}=+-_",							// [
	"[p;:'\"]}=+-_",							// {
	"}'\"[{=+\\|",								// ]
	"]'\"[{=+\\|",								// }

	"qwsz",												// a
	"awedxz",											// s
	"erfcxs",											// d
	"rtgvcd",											// f
	"tyhbvf",											// g
	"yujnbg",											// h
	"uikmnh",											// j
	"iol,<mj",										// k
	"op;:.>,<k",									// l
	":p[{'\"/?.>l",								// ;
	";p[{'\"/?.>l",								// :
	"\"[{]}/?;:",									// '
	"'[{]}/?;:",									// "

	"asx",												// z
	"zsdc",												// x
	"xdfv",												// c
	"cfgb",												// v
	"vghn",												// b
	"bhjm",												// n
	"njk,<",											// m
	"<mkl.>",											// ,
	",mkl.>",											// <
	">,<l;:/?",										// .
	".,<l;:/?",										// >
	"?.>;:'\"",										// /
	"/.>;:'\"",										// ?
};

using namespace std;

void KeyRecover::init(const char *input_,int N)
{
	uint n = strlen(input_);
	uint i;

	input = input_;
	vvv.resize(n);
	vmap.resize(n);
	for (i = 0;i < n;i ++) {
		char c = tolower(input[i]);
		char *s = strchr(keys,c);
		if (s) {
			vmap[i] = keymap[s-keys];
			vvv[i] = strlen(vmap[i]);
		} else {
			vvv[i] = 1;
			vmap[i] = &input[i];
		}
	}

	inner_loop = false;

	posgen.init(n,N);
}

bool KeyRecover::inner_step(string &output)
{
	vector<uint> vv;

	while (cgen.step(vv)) {
		output = input;
		for (uint i = 0;i < len;i ++) {
			output[v[i]] = vmap[v[i]][vv[i]];
		}
		return true;
	}
	cgen.done();
	inner_loop = false;
	return false;
}

bool KeyRecover::step(string &output)
{
	if (inner_loop) {
		if (inner_step(output))
			return true;
		// inner_step returns false, it means that we can continue the main loop
	}

	while (posgen.step(v,len)) {
		vector<uint> vv;
		vv.resize(len);
		for (uint i = 0;i < len;i ++)
			vv[i] = vvv[v[i]];
		cgen.init(vv);
		inner_loop = true;
		if (inner_step(output))
			return true;
	}
	return false;
}

void KeyRecover::done()
{
	posgen.done();
}

void keyboard_recover(const char *input,set<string> &output)
{
	KeyRecover keyr;
	string s;
	keyr.init(input);
	while (keyr.step(s)) {
		output.insert(s);
		cerr << s << endl;
	}		
	keyr.done();
	/*
	uint n = strlen(input);
	uint i,N = 2;

	vector<uint> vvv;
	vector<const char*> vmap;
	vvv.resize(n);
	vmap.resize(n);
	for (i = 0;i < n;i ++) {
		char c = tolower(input[i]);
		char *s = strchr(keys,c);
		if (s) {
			vmap[i] = keymap[s-keys];
			vvv[i] = strlen(vmap[i]);
		} else {
			vvv[i] = 1;
			vmap[i] = &input[i];
		}
	}

	PosGen posgen;
	CGen cgen;
	vector<uint> v;
	uint len;
	posgen.init(n,N);
	while (posgen.step(v,len)) {
		if (!len) continue;
		vector<uint> vv;
		vv.resize(len);
		for (i = 0;i < len;i ++)
			vv[i] = vvv[v[i]];
		cgen.init(vv);
		while (cgen.step(vv)) {
			string s = input;
			for (i = 0;i < len;i ++) {
				s[v[i]] = vmap[v[i]][vv[i]];
			}
			cerr << s << endl;
		}
		cgen.done();
	}
	posgen.done();
	*/
}

void vni_recover(const char *input,set<string> &output)
{
	string s;
	uint p,i,n = strlen(input);
	char diacritic = '0';
	bool ok;

	s = "0";											// diacritic placeholder

	for (i = 0;i < n;i ++) {
		switch (input[i]) {
		case '1': diacritic = '1'; break;
		case '2': diacritic = '2'; break;
		case '3': diacritic = '3'; break;
		case '4': diacritic = '4'; break;
		case '5': diacritic = '5'; break;

		case '6':
			ok = false;

			p = s.rfind('a');
			if (p != string::npos) {
				s[p] = 'â';
				ok = true;
			} else {
				p = s.rfind('A');
				if (p != string::npos) {
					s[p] = 'Â';
					ok = true;
				}
			}

			p = s.rfind('e');
			if (p != string::npos) {
				s[p] = 'ê';
				ok = true;
			} else {
				p = s.rfind('E');
				if (p != string::npos) {
					s[p] = 'Ê';
					ok = true;
				}
			}

			p = s.rfind('o');
			if (p != string::npos) {
				s[p] = 'ô';
				ok = true;
			} else {
				p = s.rfind('O');
				if (p != string::npos) {
					s[p] = 'Ô';
					ok = true;
				}
			}

			if (!ok)
				s += input[i];
			break;

		case '7':
			ok = false;

			p = s.rfind('u');
			if (p != string::npos) {
				s[p] = 'ß';
				ok = true;
			} else {
				p = s.rfind('U');
				if (p != string::npos) {
					s[p] = '¿';
					ok = true;
				}
			}

			p = s.rfind('o');
			if (p != string::npos) {
				s[p] = '½';
				ok = true;
			} else {
				p = s.rfind('O');
				if (p != string::npos) {
					s[p] = '´';
					ok = true;
				}
			}

			if (!ok)
				s += input[i];
			break;

		case '8':
			p = s.rfind('a');
			if (p != string::npos)
				s[p] = 'å';
			else {
				p = s.rfind('A');
				if (p != string::npos)
					s[p] = 'Å';
				else
					s += input[i];
			}
			break;

		case '9':
			p = s.rfind('d');
			if (p != string::npos)
				s[p] = 'ð';
			else {
				p = s.rfind('D');
				if (p != string::npos)
					s[p] = 'Ð';
				else
					s += input[i];
			}
			break;

		default:
			s += input[i];
		}
	}

	s[0] = diacritic;
	Syllable syll;
	if (syll.parse(s.c_str()))
		output.insert(syll.to_str());
}

void telex_recover(const char *input,set<string> &output)
{
	string s;
	uint p,i,n = strlen(input);
	char diacritic = '0';
	bool ok;

	s = "0";											// diacritic placeholder

	for (i = 0;i < n;i ++) {
		switch (tolower(input[i])) {
		case 's': diacritic = '1'; break;
		case 'f': diacritic = '2'; break;
		case 'r': diacritic = '3'; break;
		case 'x': diacritic = '4'; break;
		case 'j': diacritic = '5'; break;

		case 'w':
			ok = false;

			p = s.rfind('u');
			if (p != string::npos) {
				s[p] = 'ß';
				ok = true;
			} else {
				p = s.rfind('U');
				if (p != string::npos) {
					s[p] = '¿';
					ok = true;
				}
			}

			p = s.rfind('o');
			if (p != string::npos) {
				s[p] = '½';
				ok = true;
			} else {
				p = s.rfind('O');
				if (p != string::npos) {
					s[p] = '´';
					ok = true;
				}
			}

			p = s.rfind('a');
			if (p != string::npos) {
				s[p] = 'å';
				ok = true;
			} else {
				p = s.rfind('A');
				if (p != string::npos) {
					s[p] = 'Å';
					ok = true;
				}
			}

			if (!ok)
				s += 'ß';
			break;

		case '[': s += '½'; break;
		case ']': s += 'ß'; break;

		case 'a':
			if (!s.empty() && tolower(s[s.size()-1]) == 'a')
				s[s.size()-1] = s[s.size()-1] == 'a' ? 'â' : 'Â';
			else
				s += input[i];
			break;

		case 'e':
			if (!s.empty() && tolower(s[s.size()-1]) == 'e')
				s[s.size()-1] = s[s.size()-1] == 'e' ? 'ê' : 'Ê';
			else
				s += input[i];
			break;

		case 'o':
			if (!s.empty() && tolower(s[s.size()-1]) == 'o')
				s[s.size()-1] = s[s.size()-1] == 'o' ? 'ô' : 'Ô';
			else
				s += input[i];
			break;

		case 'd':
			if (!s.empty() && tolower(s[s.size()-1]) == 'd')
				s[s.size()-1] = s[s.size()-1] == 'd' ? 'ð' : 'Ð';
			else
				s += input[i];
			break;

		default:
			s += input[i];
		}
	}

	
	s[0] = diacritic;
	Syllable syll;
	if (syll.parse(s.c_str()))
		output.insert(syll.to_str());
}

