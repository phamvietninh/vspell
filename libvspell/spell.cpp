#include "config.h"		// -*- tab-width: 2 -*-
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include "vspell.h"
#include "sentence.h"
#include "pfs.h"
#include <map>

// stolen from glib
typedef unsigned int guint32;
typedef unsigned int guint;
typedef guint32 gunichar;
typedef char gchar;
typedef unsigned char guchar;
typedef long glong;
typedef signed int gssize;
#define UTF8_COMPUTE(Char, Mask, Len)					  \
  if (Char < 128)							      						\
    {									      										\
      Len  								= 1;								  \
      Mask 								= 0x7f;							  \
    }									      										\
  else if ((Char & 0xe0) == 0xc0)					      \
    {									      										\
      Len 								= 2;								  \
      Mask 								= 0x1f;							  \
    }									      										\
  else if ((Char & 0xf0) == 0xe0)					      \
    {									      										\
      Len 								= 3;								  \
      Mask 								= 0x0f;							  \
    }									      										\
  else if ((Char & 0xf8) == 0xf0)					      \
    {									      										\
      Len 								= 4;								  \
      Mask 								= 0x07;							  \
    }									      										\
  else if ((Char & 0xfc) == 0xf8)					      \
    {									      										\
      Len 								= 5;								  \
      Mask 								= 0x03;							  \
    }									      										\
  else if ((Char & 0xfe) == 0xfc)					      \
    {									      										\
      Len 								= 6;								  \
      Mask 								= 0x01;							  \
    }									      										\
  else									      									\
    Len 									= -1;

#define UTF8_GET(Result, Chars, Count, Mask, Len)			\
  (Result) = (Chars)[0] & (Mask);											\
  for ((Count) = 1; (Count) < (Len); ++(Count))				\
    {																									\
      if (((Chars)[(Count)] & 0xc0) != 0x80)					\
				{																							\
					(Result) = (gunichar)-1;										\
					break;																			\
				}																							\
      (Result) <<= 6;																	\
      (Result) |= ((Chars)[(Count)] & 0x3f);					\
    }

#define g_utf8_next_char(p) (char *)((p) + g_utf8_skip[*(guchar *)(p)])
static const gchar utf8_skip_data[256] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

static const gchar * const g_utf8_skip = utf8_skip_data;
static int
g_unichar_to_utf8 (gunichar c,
		   gchar   *outbuf)
{
  guint len = 0;    
  int first;
  int i;

  if (c < 0x80)
    {
      first = 0;
      len = 1;
    }
  else if (c < 0x800)
    {
      first = 0xc0;
      len = 2;
    }
  else if (c < 0x10000)
    {
      first = 0xe0;
      len = 3;
    }
   else if (c < 0x200000)
    {
      first = 0xf0;
      len = 4;
    }
  else if (c < 0x4000000)
    {
      first = 0xf8;
      len = 5;
    }
  else
    {
      first = 0xfc;
      len = 6;
    }

  if (outbuf)
    {
      for (i = len - 1; i > 0; --i)
	{
	  outbuf[i] = (c & 0x3f) | 0x80;
	  c >>= 6;
	}
      outbuf[0] = c | first;
    }

  return len;
}

static gunichar
g_utf8_get_char (const gchar *p)
{
  int i, mask = 0, len;
  gunichar result;
  unsigned char c = (unsigned char) *p;

  UTF8_COMPUTE (c, mask, len);
  if (len == -1)
    return (gunichar)-1;
  UTF8_GET (result, p, i, mask, len);

  return result;
}

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

using namespace std;

/*

The process:
1. Sentence segmentation. (sentences_split)
2. Separate "words" by spaces. (tokenize)
3. Punctuation separation. (tokenize/tokenize_punctuation)
4. Foreign/Abbreviation detection.
5. Proper name detection.
6. Generalization (into class e.g. number_class, foreign_class ...). Try to
generalize all capitalized words.
6* Syllable checking. (check1)
7. Find all possible (misspelled) words. (**) (get_all_words)
8. "pre-separate" sentence into phrases.
9. Word segmentation. (**)
10. Find the best segmentation. (segment_best)
10* Word checking. (check2)

*/
/*
	namespace Spell {
*/
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

bool VSpell::check(const char *utf8_pp)
{
	utf8_text = utf8_pp;

	syllables.clear();

	bool run = false;

	do {
		string pp = viet_to_viscii_force(utf8_text.c_str());
		vector<string> pps;
		sentences_split(pp,pps);
		unsigned pps_i,pps_len = pps.size();
		unsigned offset = 0;
		text = pp;

		for (pps_i = 0;pps_i < pps_len;pps_i ++) {
			Text *t = text_factory.create(this);
			t->offset = offset;
			run = !t->sentence_check(pps[pps_i].c_str());
			delete t;
			if (run)
				break;
			offset += pps[pps_i].size();
		}
	} while (run);
	return true;
}

void VSpell::replace(unsigned from,unsigned size,const char *s)
{
	const char *p = utf8_text.c_str();
	int i;
	for (i = 0;i < from && *p;i ++)
		p = g_utf8_next_char(p);
	unsigned from1 = p - utf8_text.c_str();
	for (i = 0;i < size && *p;i ++)
		p = g_utf8_next_char(p);
	unsigned to1 = p - utf8_text.c_str();
	utf8_text.replace(from1,to1-from1,s);
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
	if (!syllable_check() && !ui_syllable_check())
		return false;

	w.construct(st);
	//cerr << w << endl;

	PFS pfs;
	pfs.segment_best(w,seg);
	//cerr << seg << endl;
	
	// word checking
	if (!word_check() && !ui_word_check())
		return false;

	return true;									// done
}

bool Text::syllable_check()
{
	int i,n = st.get_syllable_count();

	suggestions.clear();

	for (i = 0;i < n;i ++) {
		strid id = st[i].get_cid();
		if (sarch.in_dict(id) || vspell->in_dict(st[i].get_id()))
			continue;

		st[i].cid = unk_id;

		VocabString s = sarch[id];
		if (strlen(s) == 1 && !viet_isalpha(s[0])) {
			st[i].cid = sarch["<PUNCT>"];
			continue;
		}

		Suggestion _s;
		_s.id = i;
		suggestions.push_back(_s);
	}
	return suggestions.empty();
}

bool Text::word_check()
{
	int i,n = seg.size();
	int cc = 0;

	suggestions.clear();

	for (i = 0;i < n;i ++) {
		vector<strid> sylls;
		int len = seg[i].node->get_syllable_count();
		if (len == 1) {
			cc += len;
			continue;
		} 

		int start;
		WordNodePtr node(get_root());
		for (start = 0;start < len && node != NULL; start ++)
			node = node->get_next(st[start+cc].cid);

		cc += len;
		if (node == NULL) {
			Suggestion _s;
			_s.id = i;
			suggestions.push_back(_s);
		}
	}
	return suggestions.empty();
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

void Text::replace(unsigned from,unsigned size,const char *s)
{
	vspell->replace(from+offset,size,s);
}

/*
	}
*/

static unsigned char viscii_str[] = {
	0xe1,0xe0,0xe4,0xe3,0xd5,
	0xe2,0xa4,0xa5,0xa6,0xe7,0xa7,
	0xe5,0xa1,0xa2,0xc6,0xc7,0xa3,
	0xe9,0xe8,0xeb,0xa8,0xa9,
	0xea,0xaa,0xab,0xac,0xad,
	0xae,0xed,0xec,0xef,0xee,0xb8,
	0xf3,0xf2,0xf6,0xf5,0xf7,
	0xf4,0xaf,0xb0,0xb1,0xb2,
	0xb5,0xbd,0xbe,0xb6,0xb7,0xde,
	0xfe,0xfa,0xf9,0xfc,0xfb,0xf8,
	0xdf,0xd1,0xd7,0xd8,0xe6,
	0xf1,0xfd,0xcf,0xd6,0xdb,0xdc,
	0xf0,
	0xc1,0xc0,0xc4,0xc3,0x80,
	0xc2,0x84,0x85,0x86,0x06,0x87,
	0xc5,0x81,0x82,0x02,0x05,0x83,
	0xc9,0xc8,0xcb,0x88,0x89,
	0xca,0x8a,0x8b,0x8c,0x8d,0x8e,
	0xcd,0xcc,0x9b,0xce,0x98,
	0xd3,0xd2,0x99,0xa0,0x9a,
	0xd4,0x8f,0x90,0x91,0x92,
	0x93,0xb4,0x95,0x96,0x97,0xb3,
	0x94,0xda,0xd9,0x9c,0x9d,0x9e,
	0xbf,0xba,0xbb,0xbc,0xff,0xb9,
	0xdd,0x9f,0x14,0x19,0x1e,
	0xd0,
	0
};
//"a'a`a?a~a.a^a^'a^`a^?a^~a^.a(a('a(`a(?a(~a(.e'e`e?e~e.e^e^'e^`e^?e^~e^.i'i`i?i~i.o'o`o?o~o.o^o^'o^`o^?o^~o^.o+o+'o+`o+?o+~o+.u'u`u?u~u.u+u+'u+`u+?u+~u+.y'y`y?y~y.ddA'A`A?A~A.A^A^'A^`A^?A^~A^.A(A('A(`A(?A(~A(.E'E`E?E~E.E^E^'E^`E^?E^~E^.I'I`I?I~I.O'O`O?O~O.O^O^'O^`O^?O^~O^.O+O+'O+`O+?O+~O+.U'U`U?U~U.U+U+'U+`U+?U+~U+.Y'Y`Y?Y~Y.DD";

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
map<unsigned char,gunichar> viscii_utf8_map;
map<gunichar,unsigned char> utf8_viscii_map;

void viet_init()
{
	for (unsigned i = 0;viscii_str[i];i ++) {
		viscii_utf8_map[viscii_str[i]] = unicode_str[i];
		utf8_viscii_map[unicode_str[i]] = viscii_str[i];
	}
}

bool viet_utf8_to_viscii(const char *in,char *out) // pre-allocated
{
	const gchar *p = in;
	gunichar ch;
	while ((ch = g_utf8_get_char(p)) != 0) {
		p = g_utf8_next_char(p);
		if (ch < 128) {
			*out++ = ch;
			continue;
		}

		map<gunichar,unsigned char>::iterator iter;
		iter = utf8_viscii_map.find(ch);
		if (iter != utf8_viscii_map.end())
			*out++ = (char)iter->second;
		else {
			fprintf(stderr,"Warning: unexpected unicode character %d",ch);
			return false;
		}
	}
	*out = 0;
	return true;
}

bool viet_utf8_to_viscii_force(const char *in,char *out)
{
	const gchar *p = in;
	gunichar ch;
	while ((ch = g_utf8_get_char(p)) != 0) {
		p = g_utf8_next_char(p);
		if (ch < 128) {
			*out++ = ch;
			continue;
		}

		map<gunichar,unsigned char>::iterator iter;
		iter = utf8_viscii_map.find(ch);
		if (iter != utf8_viscii_map.end())
			*out++ = (char)iter->second;
		else {
			fprintf(stderr,"Warning: unexpected unicode character %d",ch);
			*out += 'z';
		}
	}
	*out = 0;
	return true;
}

void viet_viscii_to_utf8(const char *in,char *out) // pre-allocated
{
	unsigned char *p = (unsigned char*)in;
	unsigned char ch;
	while ((ch = *p) != 0) {
		p++;
		if (ch < 128 && ch >= 32) {
			*out++ = ch;
			continue;
		}

		map<unsigned char,gunichar>::iterator iter;
		iter = viscii_utf8_map.find(ch);
		if (iter != viscii_utf8_map.end()) {
			g_unichar_to_utf8(iter->second,out);
			out = g_utf8_next_char(out);
		} else {
			*out++ = ch;							// fall-back case
		}

	}
	*out = 0;
}

static char buffer[6000];
char* viet_to_viscii(const char *in)
{
	if (g_utf8_strlen(in,-1) >= 1000)
		return "";
	if (viet_utf8_to_viscii(in,buffer))
		return buffer;
	else
		return NULL;
}

char* viet_to_viscii_force(const char *in)
{
	if (g_utf8_strlen(in,-1) >= 1000)
		return "";
	viet_utf8_to_viscii_force(in,buffer);
	return buffer;
}

char* viet_to_utf8(const char *in)
{
	if (strlen(in) >= 1000)
		return "";
	viet_viscii_to_utf8(in,buffer);
	return buffer;
}
