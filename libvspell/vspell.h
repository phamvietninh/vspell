#ifndef __VSPELL_H__ // -*- tab-width: 2 mode: c++ -*-
#define __VSPELL_H__

#ifndef __SPELL_H__
#include "spell.h"
#endif
#ifndef __SET__
#include <set>
#endif
#ifndef __STRING__
#include <string>
#endif

class VSpell;

class Text
{
protected:
	VSpell *vspell;

	virtual bool ui_syllable_check() = 0;
	virtual bool ui_word_check() = 0;

public:
	int offset;
	Sentence st;
	Lattice w;
	Segmentation seg;
	Suggestions suggestions;

	Text(VSpell *v):vspell(v) {}

	bool sentence_check(const char *pp);
	bool syllable_check();
	bool word_check();


	void replace(unsigned from,unsigned size,const char *utf8_text);

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
	void add(strid id) {
		syllables.insert(id);
	}
};


bool viet_utf8_to_viscii(const char *in,char *out); // pre-allocated
bool viet_utf8_to_viscii_force(const char *in,char *out); // pre-allocated
void viet_viscii_to_utf8(const char *in,char *out); // pre-allocated
char* viet_to_viscii(const char *in);
char* viet_to_viscii_force(const char *in);
char* viet_to_utf8(const char *in);

#endif
