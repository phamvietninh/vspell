#include <stdlib.h>    // -*- tab-width:2 coding: viscii mode: c++ -*-
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "config.h"
#include <spell.h>
#include <sentence.h>
#include <wordnode.h>
#include <pfs.h>
#include <glib.h>
using namespace std;

/*
static char *viscii_str = "áàäãÕâ¤¥¦ç§å¡¢ÆÇ£éèë¨©êª«¬­®íìïî¸óòöõ÷ô¯°±²µ½¾¶·ÞþúùüûøßÑ×ØæñýÏÖÛÜðÁÀÄÃÂÅÉÈËÊÍÌÎÓÒ Ô´³ÚÙ¿º»¼ÿ¹ÝÐ";
static gunichar unicode_str[] = {
	 225, 224,7843, 227,7841,
	 226,7845,7847,7849,7851,7853,
	 259,7855,7857,7859,7861,7863,
	 233, 232,7867,7869,7865,
	 234,7871,7873,7875,7877,
	7879, 237, 236,7881, 297,7883,
	 243, 242,7887, 245,7885,
	 244,7889,7891,7893,7895,
	7897, 417,7899,7901,7903,7905,
	7907, 250, 249,7911, 361,7909,
	 432,7913,7915,7917,7919,
	7921, 253,7923,7927,7929,7925,
	 273,
	 193, 192,7842, 195,7840,
	 194,7844,7846,7848,7850,7852,
	 258,7854,7856,7858,7860,7862,
	 201, 200,7866,7868,7864,
	 202,7870,7872,7874,7876,7878,
	 205, 204,7880, 296,7882,
	 211, 210,7886, 213,7884,
	 212,7888,7890,7892,7894,
	7896, 416,7898,7900,7902,7904,
	7906, 218, 217,7910, 360,7908,
	 431,7912,7914,7916,7918,7920,
	 221,7922,7926,7928,7924,
	 272,
	0
};

void viet_utf8_to_viscii(const gchar *in,char *out) // pre-allocated
{
	const gchar *p = in;
	gunichar ch;
	unsigned i,n = strlen(viscii_str);
	while ((ch = g_utf8_get_char(p)) != 0) {
		p = g_utf8_next_char(p);
		if (ch < 128) {
			*out++ = ch;
			continue;
		}
		for (i = 0;i < n;i ++)
			if (unicode_str[i] == ch) {
				*out++ = viscii_str[i];
				break;
			}

		if (i >= n) {
			fprintf(stderr,"Warning: unexpected unicode character %d",ch);
			*out++ = (unsigned char)ch;
		}
	}
	*out = 0;
}

void viet_viscii_to_utf8(const char *in,gchar *out) // pre-allocated
{
	unsigned char *p = (unsigned char*)in;
	unsigned char ch;
	unsigned i,n = strlen(viscii_str);
	while ((ch = *p) != 0) {
		p++;
		if (ch < 128) {
			*out++ = ch;
			continue;
		}
		for (i = 0;i < n;i ++)
			if ((unsigned char)viscii_str[i] == ch) {
				g_unichar_to_utf8(unicode_str[i],out);
				out = g_utf8_next_char(out);
				break;
			}

		if (i >= n) {
			fprintf(stderr,"Warning: unexpected viscii character %d",ch);
			g_unichar_to_utf8(ch,out);
			out = g_utf8_next_char(out);
		}
	}
	*out = 0;
}

char* viet_to_viscii(const char *in)
{
	static char buffer[1000];
	if (g_utf8_strlen(in,-1) >= 1000)
		return "";
	viet_utf8_to_viscii(in,buffer);
	return buffer;
}

char* viet_to_utf8(const char *in)
{
	static char buffer[6000];
	if (strlen(in) >= 1000)
		return "";
	viet_viscii_to_utf8(in,buffer);
	return buffer;
}
*/
class Text
{
public:
	string text;
	int offset;
	Sentence st;
	Lattice w;
	Segmentation seg;
	Suggestions suggestions;

	virtual void syllable_check() = 0;
	virtual void word_check() = 0;

	bool sentence_check(const char *pp);
	void ui_syllable_check();
	void ui_word_check();

	unsigned pos_from_syllable(const Suggestion &s);
	unsigned pos_from_word(const Suggestion &s);
};

class TextFactory
{
public:
	virtual Text* create() const = 0;
};

class VSpell
{
protected:
	const TextFactory &text_factory;
public:
	VSpell(const TextFactory &tf):text_factory(tf) {}
	virtual ~VSpell() { cleanup(); }

	bool init();
	virtual void cleanup() {}
	bool check(const string &pp);
};


bool VSpell::init()
{
	dic_init(new FuzzyWordNode(sarch["<root>"]));

	cerr << "Loading dictionary... ";
	get_root()->load("wordlist");
	cerr << "done" << endl;
	cerr << "Loading ngram... ";
	File f("ngram","rt");
	if (!f.error())
		ngram.read(f);
	else
		cerr << "Ngram loading error. The result may be incorrect" << endl;
	cerr << "done" << endl;
	sarch.set_blocked(true);
	return true;
}

bool VSpell::check(const string &pp)
{
	vector<string> pps;
	sentences_split(pp,pps);
	unsigned pps_i,pps_len = pps.size();
	unsigned offset = 0;
	for (pps_i = 0;pps_i < pps_len;pps_i ++) {
		Text *t = text_factory.create();
		t->text = pp;
		t->offset = offset;
		if (!t->sentence_check(pps[pps_i].c_str())) {
			delete t;
			return false;
		}
		delete t;
		offset += pps[pps_i].size();
	}
	return true;
}

bool Text::sentence_check(const char *pp)
{
	// preprocess
	st.set(pp);
	st.standardize();
	st.tokenize();

	if (!st.get_syllable_count())	// nothing to do but crash ;)
		return true;

	// syllable checking
	spell_check1(st,suggestions);

	// show mispelled syllables
	if (!suggestions.empty()) {
		ui_syllable_check();
		return false;
	}

	w.construct(st);
	cerr << w << endl;

	PFS pfs;
	pfs.segment_best(w,seg);
	cerr << seg << endl;
	
	// word checking
	suggestions.clear();
	spell_check2(st,seg,suggestions);

	// show mispelled words
	if (!suggestions.empty()) {
		ui_word_check();
		return false;
	}

	return true;									// done
}

void Text::ui_syllable_check()
{
	syllable_check();
}

void Text::ui_word_check()
{
	word_check();
}

unsigned Text::pos_from_syllable(const Suggestion &s)
{
	return offset+st[s.id].start;
}

unsigned Text::pos_from_word(const Suggestion &s)
{
	//return offset+st[seg[s.id]].start;
	return 0;
}
