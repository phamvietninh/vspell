#ifndef __DICTIONARY_H__ // -*- tab-width:2 mode: c++ -*-
#define __DICTIONARY_H__

#include <string>
#include <vector>
#include <cstring>
#include <srilm/Vocab.h>
#include <srilm/Ngram.h>
#include "debug.h"

namespace Dictionary {

	class WordNode;
	class Syllable;
	typedef WordNode* WordNodePtr;

	typedef VocabIndex strid;	// for migration
	typedef struct {
		strid id;			// real id
		strid cid;			// case-insensitive comparision
	} strpair;

	class StringArchive {
	private:
		Vocab dict;
		Vocab *rest;
		bool blocked;

	public:
		StringArchive():blocked(false),rest(NULL) {}
		void set_blocked(bool _blocked);
		Vocab& get_dict() { return dict; }
		void clear_rest();

		strid operator[] (VocabString s);
		strid operator[] (const std::string &s) { return (*this)[s.c_str()]; }
		VocabString operator[] (strid i);
		bool in_dict(VocabString s);
		bool in_dict(const std::string &s) { return in_dict(s.c_str()); }
		bool in_dict(strid i) { return dict.getWord(i) != 0; }
	};

	strpair make_strpair(strid id);


	class WordNode {
	protected:
		struct WordInfo {
			VocabIndex id;
			float prob;
			VocabIndex* syllables;
			int a,b;			// a/b
			WordInfo():prob(0),syllables(NULL),a(0),b(0) {}
			//      ~WordInfo() { if (syllables) delete[] syllables; }
		};
		//  WordNodePtr wl;
		typedef SArray<strid,WordNodePtr> node_map;
		typedef SArrayIter<strid,WordNodePtr> node_map_iterator;
		node_map *nodes;
		WordInfo *info;

	public:
		struct DistanceNode {
			WordNodePtr node;
			int distance;
			DistanceNode(WordNodePtr _node = NULL):node(_node),distance(0) {}
			int operator == (const DistanceNode &dn1) const {
				return dn1.node == node;
			}
			int operator < (const DistanceNode &dn1) const {
				return (int)node+distance < (int)dn1.node+dn1.distance;
			}
		};

		void recalculate();

	public:

		WordNode(strid _syllable):info(NULL),nodes(new node_map) {}
		//    ~WordNode();

		// strpair get_syllable() const { return syllable; }
		virtual WordNodePtr get_next(strid str) const;
		void inc_a() { ASSERT(info != NULL); info->a++; }
		void inc_b() { ASSERT(info != NULL); info->b++; }
		int& get_a() { ASSERT(info != NULL); return info->a; }
		int& get_b() { ASSERT(info != NULL); return info->b; }
		VocabIndex get_id() { return info->id; } // ASSERT: info != NULL
		bool is_next_empty() { return nodes->empty(); }
		virtual bool fuzzy_get_next(strid str,std::vector<DistanceNode>& _nodes) const;
		virtual WordNodePtr create_next(strid str);
		float get_prob() const { return info ? info->prob : -1; }
		void set_prob(float _prob);

		int get_syllable_count();
		void get_syllables(std::vector<strid> &syllables);
		WordNodePtr follow_syllables(const std::vector<strid> &syllables);

		bool load(const char* filename);
		bool save(const char* filename);
	};

	class FuzzyWordNode:public WordNode {
	protected:
		bool fuzzy_get_next_with_syllable(strid str,
																			std::vector<DistanceNode>& _nodes,
																			const Syllable &syll) const;
		bool fuzzy_get_next_with_ed(strid str,
																std::vector<DistanceNode>& _nodes,
																const char *str_data,
																bool parsable) const;
		bool fuzzy;

	public:
		FuzzyWordNode(strid _id):WordNode(_id),fuzzy(true) {}
		virtual bool fuzzy_get_next(strid str,std::vector<DistanceNode>& _nodes) const;
		virtual WordNodePtr create_next(strid str);
		void enable_fuzzy(bool _fuzzy) { fuzzy = _fuzzy; }
	};

	class Syllable {
	public:
		enum diacritics {
			None = 0,
			Acute,
			Grave,
			Hook,
			Tilde,
			Dot  
		};
		enum {
			First_Consonant,
			Padding_Vowel,
			Vowel,
			Last_Consonant,
			Diacritic
		};

		int components[5];
		//    int 	first_consonant;
		//    int 	padding_vowel;
		//    int 	vowel;
		//    int 	last_consonant;
		//    diacritics 	diacritic;

		Syllable(const char*  _first_consonant = NULL,
						 const char*  _padding_vowel = NULL,
						 const char*  _vowel = NULL,
						 const char*  _last_consonant = NULL,
						 int diacritic = -1);


		bool match(const Syllable &sample);
		void apply(const Syllable &sample,std::vector<Syllable> &output);
		bool parse(const char *str);
		void standardize(std::string str);
		void print();
		strid to_id();
		string to_str();
	};

	int viet_toupper(int ch);
	int viet_tolower(int ch);
	bool viet_isupper(int ch);
	bool viet_islower(int ch);

	WordNodePtr get_root();
	bool initialize(WordNodePtr _root);
	void clean();
	bool is_syllable_exist(const std::string &syll);
	float get_syllable(const std::string &syll);
	bool is_word_exist(const std::string &word);
	float get_word(const std::string &word);

	extern char *vowels[];
	extern char *first_consonants[];
	extern char *last_consonants[];
	extern char *padding_vowels[];

	extern StringArchive sarch;
	extern Ngram ngram;
	extern strid unk_id;
};


#endif
