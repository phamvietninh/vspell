#include <stdlib.h>    // -*- tab-width:2 coding: viscii mode: c++ -*-
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include "config.h"
#include <spell.h>
#include <vspell.h>
//#include <glib.h>

using namespace std;

class MyText : public Text
{
public:
	MyText(VSpell* vs):Text(vs) {}

protected:
	virtual bool ui_syllable_check();
	virtual bool ui_word_check();

	void show_wrong_syllables(unsigned);
	void show_wrong_word(unsigned);

	//	void iter_at(GtkTextIter &iter,int pos);
};

class MyTextFactory : public TextFactory
{
public:
	Text* create(VSpell *vs) const {
		return new MyText(vs);
	}
};

#define g_utf8_next_char(p) (char *)((p) + g_utf8_skip[*(unsigned char *)(p)])
static const char utf8_skip_data[256] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

static const char * const g_utf8_skip = utf8_skip_data;
typedef unsigned int guint32;
typedef unsigned int guint;
typedef guint32 gunichar;
typedef char gchar;
typedef unsigned char guchar;
typedef long glong;
typedef signed int gssize;
static glong
g_utf8_strlen (const gchar *p,
               gssize       max)
{
  glong len = 0;
  const gchar *start = p;
  //g_return_val_if_fail (p != NULL || max == 0, 0);
	if (!(p != NULL || max == 0))
		return 0;

  if (max < 0)
    {
      while (*p)
        {
          p = g_utf8_next_char (p);
          ++len;
        }
    }
  else
    {
      if (max == 0 || !*p)
        return 0;
      
      p = g_utf8_next_char (p);          

      while (p - start < max && *p)
        {
          ++len;
          p = g_utf8_next_char (p);          
        }

      /* only do the last len increment if we got a complete
       * char (don't count partial chars)
       */
      if (p - start == max)
        ++len;
    }

  return len;
}


static MyTextFactory myfactory;
static VSpell vspell(myfactory);

/*
void MyText::iter_at(GtkTextIter &iter,int pos)
{
	gtk_text_buffer_get_iter_at_offset(textbuffer_main,&iter,offset+pos);
}
*/

void MyText::show_wrong_syllables(unsigned p)
{
	const char *os = vspell->get_utf8_text().c_str();

	for (int i = 0;i < offset;i ++)
		os = g_utf8_next_char(os);

	const char *s = os;

	int i = 0,c = 0;
	int sug = 0;
	while (*s) {
		if (sug < suggestions.size()) {
			int pos = st[suggestions[sug].id].start;
			if (pos == c) {
				cout << (p == sug ? "<" : "[");
			}
		}

		const char * old_s = s;
		s = g_utf8_next_char(s);
		while (*old_s != *s) {
			cout << *old_s;
			old_s ++;
		}
		c ++;

		if (sug < suggestions.size()) {
			int pos = st[suggestions[sug].id].start;
			int pos2 = pos+strlen(get_sarch()[st[suggestions[sug].id].get_id()]);
			if (pos2 == c) {
				cout << (p == sug ? ">" : "]");
				sug ++;
			}
		}
	}
	cout << endl;
}

void MyText::show_wrong_word(unsigned p)
{
	const char *os = vspell->get_utf8_text().c_str();

	for (int i = 0;i < offset;i ++)
		os = g_utf8_next_char(os);

	const char *s = os;

	int i = 0,c = 0;
	int sug = 0;
	int stid = 0;
	int n_stid = 0;
	while (*s) {
		if (sug < suggestions.size()) {
			if (stid == 0)
				n_stid = seg[suggestions[sug].id].node->get_syllable_count();

			int st_pos = (*seg.we)[seg[suggestions[sug].id].id].pos+stid;
			int pos = st[st_pos].start;
			if (pos == c && stid == 0) {
				cout << (p == sug ? "<" : "[");
			}
		}

		const char * old_s = s;
		s = g_utf8_next_char(s);
		while (*old_s != *s) {
			cout << *old_s;
			old_s ++;
		}
		c ++;

		if (sug < suggestions.size()) {
			int st_pos = (*seg.we)[seg[suggestions[sug].id].id].pos+stid;
			int pos = st[st_pos].start;
			int pos2 = pos+strlen(get_sarch()[st[st_pos].get_id()]);
			if (pos2 == c) {
				stid ++;
				if (pos2 == c && stid == n_stid) {
					cout << (p == sug ? ">" : "]");
					if (stid >= n_stid) {
						stid = 0;
						sug ++;
					}
				}
			}
		}
	}
	cout << endl;
}

int main(int argc,char **argv)
{
	vspell.init();

	istream *in = &cin;
	ifstream ifs;
	ostream *out = &cout;
	ofstream ofs;
	
	if (argc > 1 && strcmp(argv[1],"-")) {
		ifs.open(argv[1]);
		if (!ifs.is_open()) {
			cerr << "Error open file  " << argv[1] << " for reading" << endl;
			return 0;
		}
		in = &ifs;
	}

	if (argc > 2 && strcmp(argv[2],"-")) {
		ofs.open(argv[2]);
		if (!ofs.is_open()) {
			cerr << "Error open file " << argv[2] << " for writing" << endl;
			return 0;
		}
		out = &ofs;
	}
	

	string line;
	while ((cerr << "Input: Please enter a sentence:" << endl, getline(*in,line))) {
		cout << "Checking... " << line << endl;
		vspell.check(line.c_str());
		cout << "Checked" << endl;
		(*out) << vspell.get_utf8_text();
	}

	return 0;
}



bool MyText::ui_syllable_check()
{
	unsigned i,n = suggestions.size();
	cout << "Syllable check: " << suggestions.size() << endl;
	for (i = 0;i < n;i ++) {
		show_wrong_syllables(i);
		// query
		string s;
		cerr << "Input: The right one is:" << endl;
		getline(cin,s);

		if (s.empty())
			continue;
		
		if (s == "<>")
			return true;							// force to exit

		replace(st[suggestions[i].id].start, // from
						strlen(get_sarch()[st[suggestions[i].id].get_id()]), // size
						s.c_str());					// text
		vspell->add(get_sarch()[viet_to_viscii_force(s.c_str())]);
		return false;								// continue checking
	}
}

bool MyText::ui_word_check()
{
	unsigned i,n = suggestions.size();
	cout << "Word check:" << suggestions.size()  << endl;
	for (int i = 0;i < n;i ++) {
		show_wrong_word(i);
		// query
		string s;
		cerr << "Input: The right one is:" << endl;
		getline(cin,s);

		if (s.empty())
			continue;

		if (s == "<>")
			return true;							// force to exit
		
		int count = seg[suggestions[i].id].node->get_syllable_count();
		int pos = (*seg.we)[seg[suggestions[i].id].id].pos;
		int pos2 = pos+count-1;

		string::size_type p;
		vector<unsigned> separators;
		while ((p = s.find('|')) != string::npos) {
			separators.push_back(st[pos].start+g_utf8_strlen(s.substr(0,p).c_str(),-1)+offset);
			s.erase(p,1);
		}

		replace(st[pos].start, // from
						st[pos2].start+strlen(get_sarch()[st[pos2].get_id()])-st[pos].start, // size
						s.c_str());					// text

		// add separators after replacing the text, to have old separators removed
		vspell->add_separators(separators);
		return false;								// continue checking
	}
}
