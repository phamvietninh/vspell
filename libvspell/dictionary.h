#ifndef __DICTIONARY_H__ // -*- tab-width: 2 mode: c++ -*-
#define __DICTIONARY_H__

#ifndef __STRING__
#include <string>
#endif
#ifndef __VECTOR__
#include <vector>
#endif
#ifndef __MAP__
#include <map>
#endif
extern "C" {
#include "lm/lm_3g.h"
}
#include "debug.h"

#define sarch get_ngram()
/*
	namespace Dictionary {
*/

typedef int32 VocabIndex;
typedef int32 strid;				// for easy migration
#ifndef __MYSTRING_H__
#include "mystring.h"
#endif

typedef struct {
	strid id;											// real id
	strid cid;										// case-insensitive comparision
} strpair;


class LM {
private:
	lm_t *lm;
	hash_t oov_ht;
	std::vector<const char *> oov;
	VocabIndex blocked;

public:
	bool read(const char *f);
	double wordProb(VocabIndex w1, const VocabIndex *wcontext);
	const lm_t* get_lm() const { return lm; }

public:
	LM();
	~LM();
	void set_blocked(bool _blocked);
	void clear_oov();

	strid operator[] (const char* s);
	strid operator[] (const std::string &s) { return (*this)[s.c_str()]; }
	const char* operator[] (strid i);
	bool in_dict(const char* s);
	bool in_dict(const std::string &s) { return in_dict(s.c_str()); }
	bool in_dict(strid i) { return i < lm3g_word_str_size(lm); }

	void dump();
};

//strpair make_strpair(strid id);

double LogPtoProb(double v);

int viet_toupper(int ch);
int viet_tolower(int ch);
bool viet_isupper(int ch);
bool viet_islower(int ch);
bool viet_isalpha(int ch);
bool viet_isdigit (int ch);
bool viet_isxdigit(int ch);
bool viet_isspace(int ch);
bool viet_ispunct(int ch);

bool dic_init();
void dic_clean();
bool is_syllable_exist(strid);
bool is_syllable_exist(const std::string &syll);
float get_syllable(const std::string &syll);
bool is_word_exist(const std::string &word);
float get_word(const std::string &word);

#define UNK_ID 0
#define PUNCT_ID 1
#define PROPER_NAME_ID 2
#define START_ID 3
#define STOP_ID 4
#define POEM_ID 5
#define NUMBER_ID 6
#define LEAF_ID 7
#define TOTAL_ID 8
const std::map<strid,strid_string>& get_pnames();
LM& get_ngram();
LM& get_syngram();
strid get_id(int id);

inline bool is_syllable_exist(const std::string &syll) {
	return is_syllable_exist(get_ngram()[syll]);
}

/*
	};
*/

#endif
