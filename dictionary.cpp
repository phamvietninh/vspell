#include "config.h"
#include <iostream>
#include <fstream>
#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"
#include "dictionary.h"
#include "distance.h"

using namespace std;

namespace Dictionary {
  typedef std::map<std::string,float> syllable_dict_type;
  syllable_dict_type syllable_dict;
  WordNodePtr root;

  bool initialize()
  {
    ed_init();
    root = new WordNode(string("<-root->"));
    return true;
  }
  
  void clean()
  {
    delete root;
  }

  WordNodePtr get_root()
  {
    return root;
  }

  bool is_syllable_exist(const std::string &syll)
  {
    syllable_dict_type::iterator iter = syllable_dict.find(syll);
    return (iter != syllable_dict.end());
  }

  float get_syllable(const std::string &syll)
  {
    syllable_dict_type::iterator iter = syllable_dict.find(syll);
    if(iter == syllable_dict.end())
      return 0;
    return iter->second;
  }

  bool is_word_exist(const std::string &word)
  {
    return false;
  }

  float get_word(const std::string &word)
  {
    return 0;
  }

  bool WordNode::load(const string &filename)
  {
    ifstream ifs(filename.c_str());

    if (!ifs.is_open())
      return false;

    char *buffer = new char[10240];

    ifs.rdbuf()->setbuf(buffer,10240);

    int nr_lines = 0;
    string line;
    while (getline(ifs,line)) {
      //    cerr << nr_lines << ":";
      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      tokenizer tok(line,boost::char_separator<char>("#"));
      vector<string> toks;
      copy(tok.begin(),tok.end(),back_insert_iterator<vector<string> >(toks));
      if (!toks.size())
	continue;			// unrecoverable error
      if (toks.size() < 2)
	toks.push_back("N");	// assume N
      if (toks.size() < 3)
	toks.push_back("0");	// assume 0
      tokenizer tok2(toks[0]);
      tokenizer::iterator iter;
      WordNodePtr node(this);
      for (iter = tok2.begin(); iter != tok2.end(); ++iter) {
	//      cerr << *iter << " ";
	WordNodePtr next = node->get_next(*iter);
	if (!next) {		// create new
	  next = node->create_next(*iter);
	}
	node = next;
      }
      node->set_prob(boost::lexical_cast<float>(toks[2]));
      //    cerr << endl;
      nr_lines ++;
    }
  }

  WordNodePtr WordNode::create_next(const string &str)
  {
    //  nodes[str].reset(new WordNode);
    syllable_dict[str] = 1;
    nodes[str] = new WordNode(str);
    return nodes[str];
  }

  WordNodePtr WordNode::get_next(const string &str) const
  {
    node_map::const_iterator iter = nodes.find(str);
    return iter == nodes.end() ? NULL : iter->second;
  }

  void WordNode::fuzzy_get_next(const string &str,vector<WordNodePtr>& _nodes) const
  {
    node_map::const_iterator iter;
    const char *str_data = str.data();
    int str_len = str.size();
    int ed_value;
    for (iter = nodes.begin();iter != nodes.end(); ++iter) {
#ifdef USE_EXACT_MATCH
      if (iter->first == str) {
	_nodes.push_back(iter->second);
	break;
      }
#else

      Syllable syll;
      int iter_size = iter->first.size();

      if (!syll.parse(str.c_str())) { // can't parse
	if (ED_THRESOLD2 > 0 &&
	    (iter_size - str_len >= ED_THRESOLD2 ||
	     str_len - iter_size >= ED_THRESOLD2))
	  continue;
	
	ed_value = ed(iter->first.data(),iter_size,str_data,str_len);
	//cerr << boost::format("%s <> %s => %d\n") % str % iter->first % ed_value;
	if (ed_value <= ED_THRESOLD2) {	// if (iter->first == str)
	  //cerr << iter->first << "<>" << str << " => " << ed_value << endl;
	  _nodes.push_back(iter->second);
	}
      } else {
	if (!is_syllable_exist(str)) { // parsed but not in dict
	  if (ED_THRESOLD1 > 0 &&
	      (iter_size - str_len >= ED_THRESOLD1 ||
	       str_len - iter_size >= ED_THRESOLD1))
	    continue;
	
	  ed_value = ed(iter->first.data(),iter_size,str_data,str_len);
	  //cerr << boost::format("%s <> %s => %d\n") % str % iter->first % ed_value;
	  if (ed_value <= ED_THRESOLD1) {	// if (iter->first == str)
	    //cerr << iter->first << "<>" << str << " => " << ed_value << endl;
	    _nodes.push_back(iter->second);
	  }
	} else {		// everything is right
	  // try to make some changes
	  // FIXME
	}
      }
#endif
    }
    //cerr << str << " -> " << _nodes.size() << endl;
  }

}
