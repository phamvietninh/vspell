// -*- tab-width: 2 mode: c++ -*-

#include "cgen.h"
#include "posgen.h"

#include <set>
#include <string>
#include <vector>

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

void keyboard_recover(const char *input,set<string> &output)
{
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
}
