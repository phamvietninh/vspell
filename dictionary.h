#ifndef __DICTIONARY_H__ // -*- mode: c++ -*-
#define __DICTIONARY_H__

#include <string>
#include <map>
#include <vector>

namespace Dictionary {

  class WordNode;
  typedef WordNode* WordNodePtr;

  class WordNode {
  private:
    //  WordNodePtr wl;
    typedef std::map<const std::string,WordNodePtr> node_map;
    const std::string syllable;
    node_map nodes;
    float prob;

  public:
    WordNode(const std::string &_syllable):prob(-1),syllable(_syllable) {}
    const std::string& get_syllable() const { return syllable; }
    virtual WordNodePtr get_next(const std::string &str) const;
    bool is_next_empty() { return nodes.empty(); }
    virtual void fuzzy_get_next(const std::string &str,std::vector<WordNodePtr>& _nodes) const;
    virtual WordNodePtr create_next(const std::string &str);
    float get_prob() const { return prob; }
    void set_prob(float _prob) { prob = _prob; }

    bool load(const std::string &filename);
    bool save(const std::string &filename);
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

};

#endif
