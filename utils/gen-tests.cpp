#include <iostream>
#include <vector>
#include <string>
#include <cgen.h>

using namespace std;

struct Item
{
	int pos,len;
	vector<string> candidates;
};

void load_corpus(istream &corpus,ostream &os)
{
	string s;
	while (getline(corpus,s)) {
		if (s.empty() ||s[0] == '%')
			continue;

		vector<Item> items;
		string::size_type p = 0;
		while ((p = s.find('{',p)) != string::npos) {
			string::size_type p2 = s.find('}',p);
			if (p2 == string::npos)
				continue;
			Item item;
			item.pos = p;
			item.len = p2-p+1;
			string s2 = s.substr(item.pos+1,item.len-2);
			while (!s2.empty()) {
				p = s2.find(',');
				if (p == string::npos)
					p = s2.size();
				item.candidates.push_back(s2.substr(0,p));
				s2.erase(0,p);
				while (!s2.empty() && s2[0] == ',')
					s2.erase(0,1);
			}
			items.push_back(item);
			p = p2;
		}

		CGen cg;
		vector<uint> limits,pos;
		int i,n = items.size();
		limits.resize(n);
		for (i = 0;i < n;i ++)
			limits[i] = items[i].candidates.size();

		cg.init(limits);
		while (cg.step(pos)) {
			string sentence;
			p = 0;
			for (i = 0;i < n;i ++) {
				sentence += s.substr(p,items[i].pos-p);
				p = items[i].pos+items[i].len;
				sentence += items[i].candidates[pos[i]];
			}
			sentence += s.substr(p);
			os << sentence << endl;
		}
		cg.done();
	}
}

int main(int argc,char **argv)
{
	load_corpus(cin,cout);
	return 0;
}
