#ifndef __DICTIONARY_H__ // -*- tab-width: 2 mode: c++ -*-
#define __DICTIONARY_H__

#ifndef __STRING__
#include <string>
#endif
#include <libsrilm/Vocab.h>
#include <libsrilm/Ngram.h>
#include "debug.h"

/*
	namespace Dictionary {
*/

class WordNode;
typedef WordNode* WordNodePtr;

typedef VocabIndex strid;				// for easy migration

typedef struct {
	strid id;											// real id
	strid cid;										// case-insensitive comparision
} strpair;

class StringArchive {
private:
	Vocab dict;
	Vocab *rest;
	bool blocked;

public:
	StringArchive():rest(NULL),blocked(false) {}
	void set_blocked(bool _blocked);
	Vocab& get_dict() { return dict; }
	void clear_rest();

	strid operator[] (VocabString s);
	strid operator[] (const std::string &s) { return (*this)[s.c_str()]; }
	VocabString operator[] (strid i);
	bool in_dict(VocabString s);
	bool in_dict(const std::string &s) { return in_dict(s.c_str()); }
	bool in_dict(strid i) { return i < dict.numWords(); }

	void dump();
};

//strpair make_strpair(strid id);

int viet_toupper(int ch);
int viet_tolower(int ch);
bool viet_isupper(int ch);
bool viet_islower(int ch);
bool viet_isalpha(int ch);
bool viet_isdigit (int ch);
bool viet_isxdigit(int ch);
bool viet_isspace(int ch);
bool viet_ispunct(int ch);

bool dic_init(WordNodePtr _root);
void dic_clean();
bool is_syllable_exist(strid);
bool is_syllable_exist(const std::string &syll);
float get_syllable(const std::string &syll);
bool is_word_exist(const std::string &word);
float get_word(const std::string &word);

extern StringArchive sarch;
extern Ngram ngram;
extern strid unk_id,punct_id,proper_name_id,start_id,stop_id,poem_id,number_id;

inline bool is_syllable_exist(const std::string &syll) {
	return is_syllable_exist(sarch[syll]); 
}

/*
	};
*/

#endif
