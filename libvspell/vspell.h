#ifndef __VSPELL_H__						// -*- tab-width: 2 mode: c++ -*-
#define __VSPELL_H__

#ifndef __SPELL_H__
#include "spell.h"
#endif
#ifndef __SET__
#include <set>
#endif
#ifndef __VECTOR__
#include <vector>
#endif
#ifndef __STRING__
#include <string>
#endif
#ifndef __MYSTRING_H__
#include "mystring.h"
#endif

class VSpell;

class Text
{
protected:
	VSpell *vspell;

	virtual bool ui_syllable_check() = 0;
	virtual bool ui_word_check() = 0;
	bool syllable_check(int sentence_element_id);
	void apply_separators(std::set<WordEntry> &wes);
	void get_separators(std::vector<unsigned> &seps);

public:
	int offset,length;
	Sentence st;
	Lattice w;
	Segmentation seg;
	Suggestions suggestions;

	Text(VSpell *v):vspell(v) {}
	virtual ~Text() {}

	bool sentence_check(const char *pp);
	bool syllable_check();
	bool word_check();

	void replace(unsigned from,unsigned size,const char *utf8_text);
	std::string substr(unsigned from,unsigned size);

	unsigned pos_from_syllable(const Suggestion &s);
	unsigned pos_from_word(const Suggestion &s);
};

class TextFactory
{
public:
	virtual Text* create(VSpell *) const = 0;
};


class VSpell
{
protected:
	const TextFactory &text_factory;
	std::set<strid> syllables;
	std::set<strid_string> words;
	std::vector<unsigned> separators;
	std::string utf8_text;
	std::string text;

public:
	VSpell(const TextFactory &tf):text_factory(tf) {}
	virtual ~VSpell() { cleanup(); }

	bool init();
	virtual void cleanup() {}
	bool check(const char *pp); // in utf-8 encoding

	const std::string &get_utf8_text() const { return utf8_text; }
	const std::string &get_text() const { return text; }
	void replace(unsigned from,unsigned size,const char *utf8_text);

	bool in_dict(strid id) const {
		return syllables.find(id) != syllables.end();
	}
	bool in_dict(const strid_string &id) const {
		return words.find(id) != words.end();
	}
	void add(strid id) {
		syllables.insert(id);
	}
	void add_word(const char *s);
	void add_separators(const std::vector<unsigned> &seps);
	const std::vector<unsigned> &get_separators() const { 
		return separators;
	}
};

void get_syllable_candidates(const char *input,std::set<std::string> &output);

bool viet_utf8_to_viscii(const char *in,char *out); // pre-allocated
bool viet_utf8_to_viscii_force(const char *in,char *out); // pre-allocated
void viet_viscii_to_utf8(const char *in,char *out); // pre-allocated
char* viet_to_viscii(const char *in);
char* viet_to_viscii_force(const char *in);
char* viet_to_utf8(const char *in);


#endif
