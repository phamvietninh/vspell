#include "config.h"		// -*- tab-width: 2 -*-
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include "vspell.h"
#include "sentence.h"
#include "syllable.h"
#include "pfs.h"
#include <map>
#include "propername.h"
#include "keyboard.h"
#include "shuffle.h"
#include "penalty.h"
#include "bellman.h"
#include <math.h>

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
	dic_init();

	cerr << "Loading dictionary... ";
	warch.load("wordlist");
	cerr << "done" << endl;
	cerr << "Loading ngram... ";
	File f("ngram","rt");
	if (!f.error()) {
		get_ngram().read(f);
		cerr << "done" << endl;
	} else
		cerr << "Ngram loading error. The result may be incorrect" << endl;
	cerr << "Loading syngram... ";
	File ff("syngram","rt");
	if (!ff.error()) {
		get_syngram().read(ff);
		cerr << "done" << endl;
	} else
		cerr << "Syllable Ngram loading error. The result may be incorrect" << endl;
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
			t->length = pps[pps_i].size();
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
	unsigned i;
	for (i = 0;i < from && *p;i ++)
		p = g_utf8_next_char(p);
	unsigned from1 = p - utf8_text.c_str();
	for (i = 0;i < size && *p;i ++)
		p = g_utf8_next_char(p);
	unsigned to1 = p - utf8_text.c_str();
	utf8_text.replace(from1,to1-from1,s);

	// remove separators in the range, adjust other seps
	int n = separators.size();
	if (n) {
		int newsize = g_utf8_strlen(s,-1);
		i = n;
		do {
			i --;
			if (separators[i] >= from) {
				if (separators[i] < from+size)
					separators.erase(separators.begin()+i);
				else
					separators[i] += newsize-size;
			}
		} while (i > 0);
	}
}

void VSpell::add_separators(const std::vector<unsigned> &seps)
{
	copy(seps.begin(),seps.end(),back_inserter(separators));
}

void VSpell::add_word(const char *s)
{
	istringstream is(s);
	strid_string toks;
	string ss;
	while (is >> ss) {
		toks += sarch[ss];
	}
	words.insert(toks);
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

	//w.construct(st);
	set<WordEntry> wes;
	WordStateFactories factories;
	ExactWordStateFactory exact;
	LowerWordStateFactory lower;
	UpperWordStateFactory upper;
	FuzzyWordStateFactory fuzzy;
	factories.push_back(&exact);
	factories.push_back(&lower);
	factories.push_back(&upper);
	factories.push_back(&fuzzy);
	w.pre_construct(st,wes,factories);
	mark_proper_name(st,wes);
	apply_separators(wes);
	w.post_construct(wes);
	//cerr << w << endl;

	WordDAG dagw(&w);
	DAG *dag = &dagw;

	WordDAG2 *dagw2;
	if (vspell->get_trigram()) {
		dagw2 = new WordDAG2(&dagw);
		dag = dagw2;
	}

	Penalty2DAG p2dag(dag,vspell->get_penalty2());
	if (vspell->get_penalty2()) {
		// with penalty2dag, we have to do non-fuzzy segmentation first
		// to feed Penalty2DAG::syllable_weights
		Segmentation p2seg;
		penalty2_construct(p2seg);
		p2dag.set_syllable_weights(p2seg);
		dag = &p2dag;
	}

	PenaltyDAG pdag(dag,vspell->get_penalty());
	if (vspell->get_penalty()) {
		dag = &pdag;
	}

	Path path;
	if (vspell->get_normalization()) {
		Bellman pfs;
		pfs.search(*dag,path);
	} else {
		PFS pfs;
		pfs.search(*dag,path);
	}

	if (vspell->get_trigram()) {
		dagw2->demangle(path);
		delete dagw2;
	}

	seg.resize(path.size()-2);
	// don't copy head/tail
	copy(path.begin()+1,path.end()-1,seg.begin());
	seg.we = w.we;
	//cerr << seg << endl;

	// word checking
	if (!word_check() && !ui_word_check())
		return false;

	return true;									// done
}

void Text::penalty2_construct(Segmentation &seg)
{
	WordStateFactories factories;
	ExactWordStateFactory exact;
	LowerWordStateFactory lower;
	UpperWordStateFactory upper;
	//FuzzyWordStateFactory fuzzy;

	factories.push_back(&exact);
	factories.push_back(&lower);
	factories.push_back(&upper);
	//factories.push_back(&fuzzy);
	Lattice lattice;
	set<WordEntry> wes;
	lattice.pre_construct(st,wes,factories);
	mark_proper_name(st,wes);
	//apply_separators(wes);
	lattice.post_construct(wes);

	WordDAG dagw(&lattice);
	DAG *dag = &dagw;

	WordDAG2 *dagw2;
	if (vspell->get_trigram()) {
		dagw2 = new WordDAG2(&dagw);
		dag = dagw2;
	}

	Path path;
	if (vspell->get_normalization()) {
		Bellman pfs;
		pfs.search(*dag,path);
	} else {
		PFS pfs;
		pfs.search(*dag,path);
	}

	if (vspell->get_trigram()) {
		dagw2->demangle(path);
		delete dagw2;
	}

	seg.resize(path.size()-2);
	// don't copy head/tail
	copy(path.begin()+1,path.end()-1,seg.begin());
	seg.we = lattice.we;
}


bool Text::syllable_check(int i)
{
	if (vspell->in_dict(st[i].get_id()))
		return true;

	string lcid = get_lowercased_syllable(sarch[st[i].get_cid()]);
	if (sarch.in_dict(lcid)) {
		Syllable syl;							// diacritic check
		if (syl.parse(lcid.c_str()) && syl.to_std_str() == lcid)
			return true;
		if (is_first_capitalized_word(sarch[st[i].get_id()]))
			return true;
	}
	return false;
}

bool Text::syllable_check()
{
	int i,n = st.get_syllable_count();

	suggestions.clear();

	for (i = 0;i < n;i ++) {
		if (syllable_check(i))
			continue;

		Suggestion _s;
		_s.id = i;
		suggestions.push_back(_s);
	}
	return suggestions.empty();
}

bool Text::word_check()
{
	int i,n = seg.size();

	suggestions.clear();

	for (i = 0;i < n;i ++) {
		vector<strid> sylls;
		strid_string sylls2;
		int ii,len = seg[i].node->get_syllable_count();
		seg[i].node->get_syllables(sylls);

		// case-sensitive comparation.
		sylls2.resize(len);
		bool subok = true;
		for (ii = 0;ii < len;ii ++) {
			sylls2[ii] = st[seg[i].pos+ii].get_id();
			if (subok && st[seg[i].pos+ii].get_cid() != sylls[ii])
				subok = false;
		}

		// in user dict
		bool ok = vspell->in_dict(sylls2);

		if (!ok) {
			strid_string sylls3;
			sylls3.resize(sylls.size());
			for (ii = 0;ii < sylls3.size();ii ++)
				sylls3[ii] = sarch[get_unstd_syllable(sarch[sylls[ii]])];

			if (vspell->get_strict_word_checking()) {
				// don't care if the "true" word is lower-cased and the original one is valid upper-cased
				if (!subok &&
						(is_all_capitalized_word(sylls2) ||
						 (is_first_capitalized_word(sylls2) && 
						  is_lower_cased_word(sylls3))))
					subok = true;

				ok = subok;
			} else {
				string s;
				BranchNode *branch = warch.get_root();
				LeafNode* leaf;
				for (ii = 0;ii < len && branch;ii ++)
					branch = branch->get_branch(st[seg[i].pos+ii].get_cid());

				if (branch && (leaf = branch->get_leaf(sarch["<mainleaf>"])) != NULL) {
					sylls.clear();
					leaf->get_syllables(sylls);
					strid_string sylls3;
					sylls3.resize(sylls.size());
					for (ii = 0;ii < sylls3.size();ii ++)
						sylls3[ii] = sarch[get_unstd_syllable(sarch[sylls[ii]])];

					ok = sylls2 == sylls3;

					// don't care if the "true" word is lower-cased and the original one is valid upper-cased
					if (!ok &&
							(is_all_capitalized_word(sylls2) ||
							 (is_first_capitalized_word(sylls2) &&
							  is_lower_cased_word(sylls3))))
						ok = true;
				}
			}
		}

		if (!ok) {
			Suggestion _s;
			_s.id = i;
			suggestions.push_back(_s);
			continue;
		}

		// all syllable are syntatically valid
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

int Text::utf8_pos(unsigned from)
{
	const string &utf8_text = vspell->get_utf8_text();
	const char *p = utf8_text.c_str();
	const char *op = p;
	from += offset;
	for (int i = 0;i < from && *p;i ++)
		p = g_utf8_next_char(p);
	return (int)(p - op);
}

string Text::substr(unsigned from,unsigned size)
{
	const string &utf8_text = vspell->get_utf8_text();
	const char *p = utf8_text.c_str();
	unsigned i;
	from += offset;
	for (i = 0;i < from && *p;i ++)
		p = g_utf8_next_char(p);
	unsigned from1 = p - utf8_text.c_str();
	for (i = 0;i < size && *p;i ++)
		p = g_utf8_next_char(p);
	unsigned to1 = p - utf8_text.c_str();
	return utf8_text.substr(from1,to1-from1);
}

void Text::apply_separators(set<WordEntry> &wes)
{
	vector<unsigned> seps;
	//set<unsigned> seps;

	get_separators(seps);
	sort(seps.begin(),seps.end());
	//copy(seps1.begin(),seps1.end(),inserter(seps,seps.begin()));
	int sep = 0;
	int i,n = st.get_syllable_count();

	for (i = 0;i < n-1 && sep < seps.size();i ++) {
		int p = offset+st[i].start+strlen(sarch[st[i].get_id()]);
		if (p <= seps[sep] && seps[sep] <= offset+st[i+1].start) {
			apply_separator(wes,i);
			sep ++;
		}
	}
}

void Text::get_separators(vector<unsigned> &v)
{
	const vector<unsigned> &vv = vspell->get_separators();

	int i,n = vv.size();
	for (i = 0;i < n;i ++)
		if (vv[i] >= offset && vv[i] < offset+length)
			v.push_back(vv[i]);
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

static map<unsigned char,gunichar> viscii_utf8_map;
static map<gunichar,unsigned char> utf8_viscii_map;

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

/*
static char_traits_strid::char_type*
char_traits_strid::copy(char_traits_strid::char_type* __s1,
		 const char_traits_strid::char_type* __s2,
		 size_t __n)
{
	return static_cast<char_type*>(memcpy(__s1, __s2, __n*sizeof(char_type)));
}

*/

bool get_case_syllable_candidates(const char *input,Candidates &output, float v)
{
	// There are only two acceptable cases:
	// 1. The first character is upper case, the rest is lower
	// 2. All are either lower or upper
	// if there is some upper case character without following one of these cases, then it's fault.
	// also, if there is a uppercase word in dictionary, then add it.

	uint i,n = strlen(input);
	// check for upper-case chars
	for (i = n-1;i >= 0;i --)
		if (viet_toupper(input[i]) == input[i])
			break;

	if (i <= 0 || n < 2)					// ignore if the only upper char is the first one.
		return false;

	string s;

	s = input;
	s[0] = viet_toupper(s[0]);
	for (i = 1;i < n;i ++)
		s[i] = viet_tolower(s[i]);
	if (s != string(input))
		output.insert(s,v);

	s = input;
	for (i = 0;i < n;i ++)
		s[i] = viet_tolower(s[i]);
	if (s != string(input))
		output.insert(s,v);

	s = input;
	for (i = 0;i < n;i ++)
		s[i] = viet_toupper(s[i]);
	if (s != string(input))
		output.insert(s,v);
	return true;
}

void get_phonetic_syllable_candidates(const char *input,Candidates &output,float v)
{
	vector<confusion_set>& confusion_sets = get_confusion_sets();
	int i,j,m,n = confusion_sets.size();
	bool ret = false;
	set<Syllable> syllset,syllset2;
	Syllable _syll;

	_syll.parse(input);
	syllset2.insert(_syll);
	while (!syllset2.empty()) {
		const Syllable sy = *syllset2.begin();
		syllset2.erase(syllset2.begin());

		if (syllset.find(sy) != syllset.end())
			continue;								// we already matched&applied this syllable

		//cerr << sy << endl;
		syllset.insert(sy);

		vector<Syllable> sylls;
		// match & apply
		for (i = 0;i < n;i ++) {
			m = confusion_sets[i].size();
			for (j = 0;j < m;j ++)
				if (confusion_sets[i][j].match(sy))
					break;
			if (j < m) {
				for (j = 0;j < m;j ++)
					confusion_sets[i][j].apply(sy,sylls);
			}
		}
		copy(sylls.begin(),sylls.end(), inserter(syllset2,syllset2.begin()));
	}

	// move to _nodes
	//copy(syllset.begin(),syllset.end(),ostream_iterator<Syllable>(cerr)); cerr << endl;
	set<Syllable>::iterator iter;
	for (iter = syllset.begin();iter != syllset.end(); ++ iter) {
		string s = iter->to_std_str();
		string ss = get_lowercased_syllable(s);
		//cerr << s << endl;
		if (sarch.in_dict(sarch[s]) ||
				sarch.in_dict(sarch[ss]))
			output.insert(iter->to_str(),v+1);
	}
}

void get_syllable_candidates(const char *input,Candidates &output,float v)
{
	Syllable syll;
	string s,s2;
	set<string> s3;
	set<string>::iterator s3i;


	// bo dau sai vi tri
	if (syll.parse(input) &&
			syll.to_str() != string(input))
		output.insert(syll.to_str(),v+10);

	get_phonetic_syllable_candidates(input,output,v);

	KeyRecover keyr;
	keyr.init(input);
	while (keyr.step(s)) {
		s2 = get_std_syllable(s);
		if (s2 != s && syll.parse(s2.c_str()))
			output.insert(syll.to_str(),v);
		s3.clear();
		im_recover(s.c_str(),s3);
		for (s3i = s3.begin(); s3i != s3.end(); ++ s3i) {
			s2 = get_std_syllable(*s3i);
			if (s2 != *s3i && syll.parse(s2.c_str()))
				output.insert(syll.to_str(),v);
		}
	}
	keyr.done();

	// SpaceInserter
	uint i,n = strlen(input);
	for (i = 1;i < n;i ++) {
		s = string(input).substr(0,i);
		s2 = string(input).substr(i);
		if (syll.parse(s.c_str())) {
			s = syll.to_str();
			if (syll.parse(s2.c_str())) {
				output.insert(s + string(" ") + syll.to_str(),v);
			}
		}
	}

	CharInserter inserter;
	inserter.init(input);
	while (inserter.step(s)) {
		s2 = get_std_syllable(s);
		if (s2 != s && syll.parse(s2.c_str()))
			output.insert(syll.to_str(),v);
		s3.clear();
		im_recover(s.c_str(),s3);
		for (s3i = s3.begin(); s3i != s3.end(); ++ s3i) {
			s2 = get_std_syllable(*s3i);
			if (s2 != *s3i && syll.parse(s2.c_str()))
				output.insert(syll.to_str(),v);
		}
	}
	inserter.done();

	CharEraser eraser;
	eraser.init(input);
	while (eraser.step(s)) {
		s2 = get_std_syllable(s);
		if (s2 != s && syll.parse(s2.c_str()))
			output.insert(syll.to_str(),v);
		s3.clear();
		im_recover(s.c_str(),s3);
		for (s3i = s3.begin(); s3i != s3.end(); ++ s3i) {
			s2 = get_std_syllable(*s3i);
			if (s2 != *s3i && syll.parse(s2.c_str()))
				output.insert(syll.to_str(),v);
		}
	}
	eraser.done();

	CharTransposer transposer;
	transposer.init(input);
	while (transposer.step(s)) {
		s2 = get_std_syllable(s);
		if (s2 != s && syll.parse(s2.c_str()))
			output.insert(syll.to_str(),v);
		s3.clear();
		im_recover(s.c_str(),s3);
		for (s3i = s3.begin(); s3i != s3.end(); ++ s3i) {
			s2 = get_std_syllable(*s3i);
			if (s2 != *s3i && syll.parse(s2.c_str()))
				output.insert(syll.to_str(),v);
		}
	}
	transposer.done();
}

void get_left_syllable_candidates(const char *input,const char *left,Candidates &output)
{
	// merge
	string s;
	s = string(left)+string(input);
	get_syllable_candidates(s.c_str(),output,10);

	// cut one char from input
	if (strlen(input) > 1) {
		s = string(input+1);
		get_syllable_candidates(s.c_str(),output,10);
	}

	// insert one char from left to input
	if (strlen(left)) {
		s = string(" ") + string(input);
		s[0] = left[strlen(left)-1];
		get_syllable_candidates(s.c_str(),output,10);
	}
}

void get_right_syllable_candidates(const char *input,const char *right,Candidates &output)
{
	// merge
	string s;
	s = string(input)+string(right);
	get_syllable_candidates(s.c_str(),output,10);

	// cut one char from input
	if (strlen(input) > 1) {
		s = string(input);
		s.resize(strlen(input)-1);
		get_syllable_candidates(s.c_str(),output,10);
	}

	// insert one char from right to input
	if (strlen(right)) {
		s = string(input)+string(" ");
		s[s.size()-1] = right[0];
		get_syllable_candidates(s.c_str(),output,10);
	}
}



void Candidates::insert(const std::string &s,float f)
{
	Candidate c;
	c.candidate = s;
	c.priority = f;
	set<Candidate>::iterator iter = candidates.find(c);
	if (iter != candidates.end()) {
		if (iter->priority < c.priority)
			candidates.erase(iter);
		else
			return;
	}
	candidates.insert(c);
}

bool Candidates::CandidateComparator::operator()(const std::string &s1,const std::string &s2)
{
	set<Candidate>::iterator i1,i2;
	Candidate c1,c2;
	c1.candidate = s1;
	c2.candidate = s2;
	i1 = c.candidates.find(c1);
	i2 = c.candidates.find(c2);
	if (i1->priority != i2->priority)
		return i1->priority > i2->priority;
	float f1,f2;
	VocabIndex v;
	v = Vocab_None;
	f1 = -get_syngram().wordProb(sarch[get_std_syllable(get_lowercased_syllable(s1))],&v);
	f2 = -get_syngram().wordProb(sarch[get_std_syllable(get_lowercased_syllable(s2))],&v);
	//cerr << f1 << "<>" << f2 << endl;
	return f1 > f2;	// we want reverse order
}

void Candidates::get_list(std::vector<std::string> &v)
{
	set<Candidate>::iterator iter;

	iter = candidates.begin();
	while (iter != candidates.end()) {
		if (!is_valid_word_form(iter->candidate.c_str())) {
			get_case_syllable_candidates(iter->candidate.c_str(),*this,iter->priority);
			candidates.erase(iter++);
		} else
			++iter;
	}

	v.resize(candidates.size());
	uint n = 0;
	for (iter = candidates.begin();iter != candidates.end(); ++iter)
		if (sarch.in_dict(get_std_syllable(get_lowercased_syllable(iter->candidate))))
			v[n++] = iter->candidate;
	v.resize(n);
	sort(v.begin(),v.end(),CandidateComparator(*this));
}
