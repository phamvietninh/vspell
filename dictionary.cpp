#include "config.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"
#include "dictionary.h"
#include "distance.h"
#ifdef TRAINING
#include <math.h>
#endif

using namespace std;

namespace Dictionary {

  typedef std::map<strid,float> syllable_dict_type;
  syllable_dict_type syllable_dict;
  WordNodePtr root;

  StringArchive sarch;

  bool syllable_init();

  bool initialize()
  {
    syllable_init();
    ed_init();
    root = new WordNode(sarch["<-root->"]);
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

  bool is_syllable_exist(strid syll)
  {
    syllable_dict_type::iterator iter = syllable_dict.find(syll);
    return (iter != syllable_dict.end());
  }

  float get_syllable(strid syll)
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

  bool WordNode::load(const char* filename)
  {
    ifstream ifs(filename);

    if (!ifs.is_open())
      return false;

    char *buffer = new char[10240];

    ifs.rdbuf()->setbuf(buffer,10240);

    int nr_lines = 0;
    string line;
    while (getline(ifs,line)) {
      //cerr << nr_lines << ":" << line << endl;
      int pos,start,len;
      start = 0;
      len = line.size();
      vector<string> toks;
      while (start < len) {
	pos = line.find('#',start);
	if (pos == string::npos)
	  pos = len;
	toks.push_back(line.substr(start,pos-start));
	start = pos+1;
      }

      if (!toks.size())
	continue;			// unrecoverable error
      if (toks.size() < 2)
	toks.push_back("N");	// assume N
      if (toks.size() < 3)
	toks.push_back("0");	// assume 0

      start = 0;
      len = toks[0].size();
      WordNodePtr node(this),cnode(this);
#ifdef TRAINING
      vector<strid> tr_node;
#endif
      while (start < len) {
	pos = toks[0].find(' ',start);
	if (pos == string::npos)
	  pos = len;
	string s = toks[0].substr(start,pos-start);
	int id = sarch[s];
	WordNodePtr next = node->get_next(id);
	if (!next) {		// create new
	  next = node->create_next(id);
	}
#ifdef TRAINING
	tr_node.push_back(id);
#endif
	node = next;

	transform(s.begin(),s.end(),s.begin(),viet_tolower);
	int cid = sarch[s];
	WordNodePtr cnext = cnode->get_next(cid);
	if (!cnext) {		// create new
	  cnext = cnode->create_next(cid);
	}
	cnode = cnext;
	start = pos+1;
      }
      if (toks[2].find(' ') != string::npos)
	toks[2].erase(toks[2].find(' '));
      node->set_prob(boost::lexical_cast<float>(toks[2]));
      cnode->set_prob(boost::lexical_cast<float>(toks[2]));
      nr_lines ++;
    }
  }

  bool WordNode::save(const char* filename)
  {
    ofstream ofs(filename);

    if (!ofs.is_open())
      return false;

    typedef pair<WordNodePtr,vector<strid> > trace_type;
    vector<trace_type> traces;
    traces.push_back(make_pair(this,vector<strid>()));

    while (!traces.empty()) {
      trace_type trace = traces.back();
      traces.pop_back();
      node_map::iterator iter;
      WordNodePtr trace_ptr = trace.first;
      for (iter = trace_ptr->nodes.begin();iter != trace_ptr->nodes.end(); ++iter) {
	trace_type newtrace = make_pair(iter->second,trace.second);
	newtrace.second.push_back(iter->first);
	traces.push_back(newtrace);
      }

      if (trace_ptr->get_prob() >= 0) {
	int i,n = trace.second.size();
	for (i = 0;i < n;i ++) {
	  if (i) ofs << " ";
	  ofs << sarch[trace.second[i]];
	}
	ofs << "#" << "N" << "#" << trace_ptr->get_prob() << endl;
      }
    }

  }

  WordNodePtr WordNode::create_next(strid str)
  {
    WordNodePtr ptr;
    //  nodes[str].reset(new WordNode);
    syllable_dict[str] = 1;
    ptr = nodes[str] = new WordNode(str);
    ptr->parent = this;
    return ptr;
  }

  WordNodePtr WordNode::get_next(strid str) const
  {
    node_map::const_iterator iter = nodes.find(str);
    return iter == nodes.end() ? NULL : iter->second;
  }

  int WordNode::get_syllable_count()
  {
    int i = 0;			// we dont like <-root->
    WordNodePtr p = parent;
    //    cerr << "<" << sarch[syllable.id] << "> ";
    while (p != NULL) {
      //      cerr << "<" << sarch[p->syllable.id] << ">";
      p = p->parent;
      i ++;
    }
    return i;
  }

  void WordNode::get_syllables(vector<strid> &syllables)
  {
    WordNodePtr p = this;
    do {
      syllables.push_back(p->syllable.id);
      p = p->parent;
    } while (p != NULL && p->parent != NULL);
  }

  bool WordNode::fuzzy_get_next(strid str,vector<DistanceNode>& _nodes) const
  {
    node_map::const_iterator iter;
#ifdef USE_EXACT_MATCH
    iter = nodes.find(str);
    if (iter != nodes.end()) {
      _nodes.push_back(iter->second);
      return true;
    } else
      return false;
#else
    const char *str_data = sarch[str];
    int str_len = strlen(str_data);
    int ed_value;
    bool ret = false;

    for (iter = nodes.begin();iter != nodes.end(); ++iter) {

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
	  _nodes.back().distance = ed_value;
	  ret = true;
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
	    _nodes.back().distance = ed_value;
	    ret = true;
	  }
	} else {		// everything is right
	  // try to make some changes

	  // hard coding these is bad or good?
	  /*
	    if (sys.first_consonant != -1 &&
	    (!strcmp(first_consonants[syll.first_consonant],"c") || 
	    !strcmp(first_consonants[syll.first_consonant],"k"))) {
	    values[0][0] = 10;
	    values[0][1] = 15;
	    limit[0] = 2;
	    } else if (sys.first_consonant != -1 &&
	    (!strcmp(first_consonants[syll.first_consonant],"g") || 
	    !strcmp(first_consonants[syll.first_consonant],"gh"))) {
	    values[0][0] = 4;
	    values[0][1] = 19;
	    limit[0] = 2;
	    } else if (sys.first_consonant != -1 &&
	    (!strcmp(first_consonants[syll.first_consonant],"ng") || 
	    !strcmp(first_consonants[syll.first_consonant],"ngh"))) {
	    values[0][0] = 2;
	    values[0][1] = 0;
	    limit[0] = 2;
	    } else if (sys.first_consonant != -1 &&
	    (!strcmp(first_consonants[syll.first_consonant],"ch") || 
	    !strcmp(first_consonants[syll.first_consonant],"tr"))) {
	    values[0][0] = 3;
	    values[0][1] = 7;
	    limit[0] = 2;
	    } else if (sys.first_consonant != -1 &&
	    (!strcmp(first_consonants[syll.first_consonant],"s") || 
	    !strcmp(first_consonants[syll.first_consonant],"x"))) {
	    values[0][0] = 23;
	    values[0][1] = 25;
	    limit[0] = 2;
	    } else if (sys.first_consonant != -1 &&
	    (!strcmp(first_consonants[syll.first_consonant],"v") || 
	    !strcmp(first_consonants[syll.first_consonant],"d") ||
	    !strcmp(first_consonants[syll.first_consonant],"gi") || 
	    !strcmp(first_consonants[syll.first_consonant],"r"))) {
	    values[0][0] = 24;
	    values[0][1] = 17;
	    values[0][2] = 8;
	    values[0][3] = 22;
	    limit[0] = 4;
	    } else if (syll.first_consonant == -1 ||
	    !strcmp(first_consonants[syll.first_consonant],"ng") ||
	    !strcmp(first_consonants[syll.first_consonant],"qu")) {
	    values[0][0] = 24;
	    values[0][1] = 17;
	    values[0][2] = 8;
	    values[0][3] = 22;
	    limit[0] = 4;
	    } else {
	    values[0][0] = syll.first_consonant;
	    limit[0] = 1;
	    }
	  */
	}
      }
    }
    return ret;
    //cerr << str << " -> " << _nodes.size() << endl;
#endif
  }

#ifdef TRAINING
  void WordNode::recalculate()
  {
    vector<WordNodePtr> traces;
    traces.push_back(this);

    while (!traces.empty()) {
      WordNodePtr trace = traces.back();
      traces.pop_back();
      node_map::iterator iter;
      WordNodePtr trace_ptr = trace;
      for (iter = trace_ptr->nodes.begin();
	   iter != trace_ptr->nodes.end();
	   ++iter) {
	WordNodePtr newtrace = iter->second;
	traces.push_back(newtrace);
      }

      if (trace_ptr->get_prob() >= 0) {
	if (trace_ptr->a > 0 && trace_ptr->b > 0)
	  trace_ptr->prob = -log10((double)trace_ptr->a/(double)trace_ptr->b);
	else
	  trace_ptr->prob = -log10(1.0/1000.0);
	if (trace_ptr->prob == -0) trace_ptr->prob = 0;
	trace_ptr->a = trace_ptr->b = 0;
      }
    }

  }
#endif

  strid StringArchive::operator[] (const char *s)
  {
    a2i_type::iterator iter = a2i.find(s);
    if (iter == a2i.end()) {	// add new
      int n = i2a.size();
      char* ns = strdup(s);
      i2a.push_back(ns);
      a2i[ns] = n;
      return n;
    }
    return iter->second;
  }

  const char * StringArchive::operator[] (strid i)
  {
    if (i >= i2a.size())
      return NULL;
    return i2a[i];
  }

  StringArchive::StringArchive(int size)
  {
    i2a.reserve(size);
    (*this)["<-start->"];	// insert start id
    (*this)["<-end->"];		// insert end id
  }

  StringArchive::~StringArchive()
  {
    int i,n = i2a.size();
    for (i = 0;i < n;i ++)
      free((void*)i2a[i]);	// allocated by strdup()
  }

  strpair make_strpair(strid str)
  {
    const char *s = sarch[str];
    string st(s);
    int i,len = st.size();
    for (i = 0;i < len;i ++) {
      st[i] = viet_tolower(st[i]);
    }
    strpair pair;
    pair.id = str;
    pair.cid = sarch[st];
    return pair;
  }
}
