#include <stdlib.h> // -*- tab-width:2 coding: viscii mode: c++ -*-
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include "config.h"
#include <spell.h>
#include <vspell.h>
#include <syllable.h>
#include <cgen.h>
#include <boost/format.hpp>
#include <set>

using namespace std;

class MyText : public Text
{
public:
	MyText(VSpell* vs):Text(vs) {}

	bool word_check();
	bool syllable_check();

protected:
	virtual bool ui_syllable_check();
	virtual bool ui_word_check();
	string word_to_utf8(unsigned seg_id);
};

class MyTextFactory : public TextFactory
{
public:
	Text* create(VSpell *vs) const {
		return new MyText(vs);
	}
};

static MyTextFactory myfactory;
static VSpell vspell(myfactory);

struct Item
{
	int pos,len;
	vector<string> candidates;
};

typedef vector<Item> Items;

struct Pattern
{
	int trigram,normalization,strich_checking;
	float penalty,penalty2;
	friend ostream& operator << (ostream &os,const Pattern &p) {
		os << p.trigram 
			 << "_" << p.normalization
			 << "_" << p.strich_checking
			 << "_" << p.penalty
			 << "_" << p.penalty2;
		return os;
	}
	friend istream& operator >> (istream &is,Pattern &pat) {
		is >> pat.trigram >> pat.normalization >> pat.strich_checking >> pat.penalty >> pat.penalty2;
		return is;
	}
};

struct Test
{
	string sentence;
	Items items;
	vector<uint> pos;
	vector<uint> len;
	set<uint> error;
	int corrects,positives,candidates;
	bool syllable_checked,word_checked;
};

static Test* mytest;
static ostream *os;
	vector<Test> tests;

void check_pattern(Pattern &pat)
{
	vspell.set_penalty(pat.penalty);
	vspell.set_penalty2(pat.penalty2);
	vspell.set_normalization(pat.normalization);
	vspell.set_trigram(pat.trigram);
	vspell.set_strict_word_checking(pat.strich_checking);
	uint i_corpus,n_corpus = tests.size();
	cerr << "Pattern " << pat;
	ostringstream oss;
	oss << "pattern." << pat;
	os = new ofstream(oss.str().c_str());
	(*os) << "#Pattern " << pat << endl;
	for (i_corpus = 0;i_corpus < n_corpus;i_corpus ++) {
		mytest = &tests[i_corpus];
		mytest->corrects = mytest->positives = mytest->candidates = 0;
		mytest->syllable_checked = mytest->word_checked = false;
		//(*os) << "#Sentence: " << mytest->sentence << endl;
		vspell.check(tests[i_corpus].sentence.c_str());
		(*os) << boost::format("%d %d %d %d %d %d") %
			mytest->syllable_checked %
			mytest->word_checked %
			mytest->corrects %
			mytest->error.size() %
			mytest->positives %
			mytest->candidates
			<< endl;
	}
	delete os;
}

void save_corpus(const char *filename)
{
	ofstream corpus(filename);

	if (!corpus.is_open()) {
		cerr << "could not open file" << endl;
		return;
	}

	uint i,n = tests.size();
	for (i = 0;i < n;i ++) {
		if (tests[i].error.empty())
			corpus << endl;
		corpus << tests[i].sentence << endl;
	}
}
void load_corpus(const char *filename)
{
	ifstream corpus(filename);

	if (!corpus.is_open()) {
		cerr << "could not open file" << endl;
		return;
	}

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
			Test test;
			test.pos.resize(n);
			test.len.resize(n);
			p = 0;
			for (i = 0;i < n;i ++) {
				test.sentence += s.substr(p,items[i].pos-p);
				p = items[i].pos+items[i].len;
				test.pos[i] = test.sentence.size();
				test.sentence += items[i].candidates[pos[i]];
				test.len[i] = items[i].candidates[pos[i]].size();
				if (pos[i]) test.error.insert(i);
			}
			test.sentence += s.substr(p);
			test.items = items;
			tests.push_back(test);
		}
		cg.done();
	}
	cerr << "Tests: " << tests.size() << endl;
}

int main(int argc,char **argv)
{
	string s;

	/*
	vector<Pattern> patterns;

	ifstream rules("vspell-check.rules");
	if (rules.is_open()) {
		while (getline(rules,s)) {
			if (s.empty() || s[0] == '#')
				continue;
			Pattern pat;
			if (sscanf(s.c_str(),"%d %d %d %f",
								 &pat.trigram,
								 &pat.normalization,
								 &pat.strich_checking,
								 &pat.penalty) == 4)
				patterns.push_back(pat);
			else
				cerr << "Error pattern " << s << endl;
		}
	}
	cerr << "Patterns: " << patterns.size() << endl;
	*/

	vspell.init();
	/*
	uint i_pat,n_pat = patterns.size();
	for (i_pat = 0;i_pat < n_pat;i_pat ++) {
		Pattern &pat = patterns[i_pat];
		check_pattern(pat);
	}
	*/
	Pattern pat;
	while (cin >> s) {
		if (s == "load") {
			string filename;
			cin >> filename;
			load_corpus(filename.c_str());
		} else if (s == "empty") {
			tests.clear();
		} else if (s == "run" ) {
			cin >> pat;
			check_pattern(pat);
		} else if (s == "save") {
			string filename;
			cin >> filename;
			save_corpus(filename.c_str());
		} else
			cerr << "unknown command.";
		cerr << "done" << endl;
	}
}

bool MyText::ui_syllable_check()
{
	unsigned i,n = suggestions.size();
	unsigned ii,nn;
	for (i = 0;i < n;i ++) {
		int from,len;
		from = st[suggestions[i].id].start;
		len = strlen(get_sarch()[st[suggestions[i].id].id]);
		int utf8_from,utf8_len;
		utf8_from = utf8_pos(from);
		utf8_len = utf8_pos(from+len)-utf8_from;
		nn = mytest->items.size();
		for (ii = 0;ii < nn;ii ++)
			if (mytest->error.find(ii) != mytest->error.end() &&
					utf8_from == mytest->pos[ii] &&
					utf8_len == mytest->len[ii])
				break;
		if (ii == nn) {
			mytest->positives ++;
			/*
			(*os) << "#Syllable " << utf8_from << "-" << utf8_len 
					 << "(" << from << "-" << len << ")" << endl;
			for (ii = 0;ii < nn;ii ++)
				(*os) << "# " << mytest->pos[ii] << "-" << mytest->len[ii] << endl;
			*/
			continue;
		}

		/*
		vector<string> candidates;
		Candidates c;
		candidates_reset();
		get_syllable_candidates(get_sarch()[st[suggestions[i].id].id],c);
		c.get_list(candidates);
		*/
		mytest->corrects ++;
	}
	mytest->syllable_checked = true;
	return true;
}

bool MyText::ui_word_check()
{
	unsigned i,n = suggestions.size();
	int pos,pos2,count;
	unsigned ii,nn;

	for (i = 0;i < n;i ++) {
		// query
		count = seg[suggestions[i].id].node->get_syllable_count();
		pos = (*seg.we)[seg[suggestions[i].id].id].pos;
		pos2 = pos+count-1;
		int from,len;
		from = st[pos].start;
		len = st[pos2].start+strlen(get_sarch()[st[pos2].id])-from;
		int utf8_from,utf8_len;
		utf8_from = utf8_pos(from);
		utf8_len = utf8_pos(from+len)-utf8_from;
		nn = mytest->items.size();
		for (ii = 0;ii < nn;ii ++)
			if (mytest->error.find(ii) != mytest->error.end() &&
					utf8_from <= mytest->pos[ii] &&
					utf8_from + utf8_len >= mytest->pos[ii] + mytest->len[ii])
				break;
		if (ii == nn) {
			mytest->positives ++;
			/*
			(*os) << "#Word " << utf8_from << "-" << utf8_len 
					 << "(" << from << "-" << len << ")"
					 << endl;
			*/
			continue;
		}

		string s = mytest->sentence.substr(utf8_from,utf8_len);
		s.replace(mytest->pos[ii]-utf8_from,mytest->len[ii],mytest->items[ii].candidates[0]);

		if (s == word_to_utf8(suggestions[i].id).c_str())
			mytest->corrects ++;
		else  {
			mytest->candidates ++;
			(*os) << boost::format("#Word2 %d-%d(%d-%d) %s-%s (%s)" ) %
				utf8_from %
				utf8_len %
				from %
				len %
				mytest->items[ii].candidates[0] %
				word_to_utf8(suggestions[i].id) %
				s
						<< endl;
		}
	}
	mytest->word_checked = true;
	return true;
}

bool MyText::word_check()
{
	bool ret = Text::word_check();
	return ret;
}

bool MyText::syllable_check()
{
	bool ret = Text::syllable_check();
	return ret;
}

string MyText::word_to_utf8(unsigned seg_id)
{
	vector<strid> sylls;
	string s;
	seg[seg_id].node->get_syllables(sylls);
	int i,n = sylls.size();
	for (i = 0;i < n;i ++) {
		if (i)
			s += " ";
		Syllable syll;
		syll.parse(get_sarch()[sylls[i]]);
		s += viet_to_utf8(syll.to_str().c_str());
	}
	return s;
}

