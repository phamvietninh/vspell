#ifndef __DICTIONARY_H__ // -*- mode: c++ -*-
#define __DICTIONARY_H__

#include <string>
#include <map>
#include <vector>
#include <cstring>

namespace Dictionary {

  class WordNode;
  typedef WordNode* WordNodePtr;

  typedef int strid;
  typedef struct {
    strid id;			// real id
    strid cid;			// case-insensitive comparision
  } strpair;

  class StringArchive {
  private:
    class StringCompare {
    public:
      bool operator() (const char *s1,const char *s2) {
	return strcmp(s1,s2) < 0;
      }
    };
    typedef std::vector<const char*> i2a_type;
    typedef std::map<const char*,strid,StringCompare> a2i_type;
    i2a_type i2a;		// id -> string
    a2i_type a2i;		// string -> id

  public:
    strid operator[] (const char *s);
    strid operator[] (const std::string &s) { return (*this)[s.c_str()]; }
    const char * operator[] (strid i);
    StringArchive(int size = 10000); // the bigger, the better :-)
    ~StringArchive();
  };

  strpair make_strpair(strid id);


  class WordNode {
  private:
    //  WordNodePtr wl;
    typedef std::map<strid,WordNodePtr> node_map;
    strpair syllable;
    node_map nodes;
    float prob;
    WordNodePtr parent;

  public:
    struct DistanceNode {
      WordNodePtr node;
      int distance;
      DistanceNode(WordNodePtr _node = NULL):node(_node),distance(0) {}
    };

#ifdef TRAINING
    int a,b;			// a/b
    void recalculate();
#endif

  public:

    WordNode(strid _syllable):
      prob(-1),
      syllable(make_strpair(_syllable)),
      parent(NULL) {
#ifdef TRAINING
      a = b = 0;
#endif
    }
    strpair get_syllable() const { return syllable; }
    virtual WordNodePtr get_next(strid str) const;
    bool is_next_empty() { return nodes.empty(); }
    virtual bool fuzzy_get_next(strid str,std::vector<DistanceNode>& _nodes) const;
    virtual WordNodePtr create_next(strid str);
    float get_prob() const { return prob; }
    void set_prob(float _prob) { prob = _prob; }

    int get_syllable_count();
    void get_syllables(vector<strid> &syllables);

    bool load(const char* filename);
    bool save(const char* filename);
  };

  class Syllable {
  public:
    enum diacritics {
      none = 0,
      acute,
      grave,
      hook,
      tilde,
      dot  
    };

    int 		first_consonant;
    int 		padding_vowel;
    int 		vowel;
    int 		last_consonant;
    diacritics 	diacritic;

    bool parse(const char *str);
    void standardize(std::string str);
  };

  int viet_toupper(int ch);
  int viet_tolower(int ch);
  bool viet_isupper(int ch);
  bool viet_islower(int ch);

  WordNodePtr get_root();
  bool initialize();
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
};


#endif
