#include "wfst.h"
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"

using namespace std;

void Sentence::standardize()
{
}

void Sentence::tokenize()
{
  /*
  using namespace boost;
  typedef tokenizer<char_delimiters_separator<char>,std::string::iterator> mytokenizer;
  mytokenizer tok(sent_->sent_);
  mytokenizer::iterator iter;
  for (iter = tok.begin();iter != tok.end(); ++iter) {
    Syllable sy;
    sy.start = iter->begin();
    sy.start = iter->end();
  }
  */
  string &st = sent_;
  int pos = 0,len = st.size();
  bool started = false;		// we have found start of a new word
  Syllable sy;
  sy.span = 1;
  sy.sent_ = this;

  while (pos < len) {
    if (started) {
      pos = st.find(' ',sy.start);
      if (pos == string::npos)
	pos = len;
      sy.end = pos;
      syllables.push_back(sy);
      started = false;
    } else {
      if (st[pos] == ' ')
	pos++;
      else {
	started = true;
	sy.start = pos;
      }
    }
  }
}

bool WordNode::load(const string &filename)
{
  ifstream ifs(filename.c_str());

  if (!ifs.is_open())
    return false;

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
  nodes[str] = new WordNode;
  return nodes[str];
}

WordNodePtr WordNode::get_next(const string &str) const
{
  node_map::const_iterator iter = nodes.find(str);
  return iter == nodes.end() ? NULL : iter->second;
}

/*
std::string& Sentence::const_iterator::operator++()
{
}

std::string Sentence::const_iterator::operator++(int)
{
}
*/

class SegmentationComparator
{
public:
  bool operator() (const WFST::Segmentation &seg1,const WFST::Segmentation &seg2) {
    return seg1.prob < seg2.prob;
  }
};

void WFST::segment_best(const Sentence &_sent,Segmentation &seps)
{
  vector<Segmentation> segms;
  insert_iterator<vector<Segmentation> > iter(segms,segms.begin());
  segment_all(_sent,iter);

  // sort by prob
  sort(segms.begin(),segms.end(),SegmentationComparator());

  for (vector<Segmentation>::iterator i = segms.begin();i != segms.end(); ++i) {
    cerr << endl;
    for (vector<Separator>::iterator ii = i->sep_list.begin();ii != i->sep_list.end(); ++ii) {
      cerr << ii->anchor << " ";
    }
  }

}

struct Trace
{
  int next;
  WFST::Segmentation segm;
  WordNodePtr node;
};

template <class OutputIterator>
void WFST::segment_all(const Sentence &sent,OutputIterator iter)
{

  int nr_syllables = sent.get_syllable_number();
  int i;
  vector<Trace> traces;
  
  Trace start_trace;
  start_trace.next = 0;
  start_trace.node = wl;

  traces.push_back(start_trace);

  while (!traces.empty()) {
    // get one
    Trace &trace = traces.back();

    if (trace.next == nr_syllables) {
      traces.pop_back();
      continue;
    }

    Sentence::Syllable syll = sent[trace.next];
    cerr << *syll << endl;

    WordNodePtr next_node(trace.node->get_next(*syll));
    if (!next_node) {
      traces.pop_back();	// cancel this trace
      continue;
    }

    float prob = next_node->get_prob();
    if (prob < 0)
      continue;			// nothing happens, keep going

    // Have prob, create a new trace and push back this trace.
    int cur = trace.next;
    
    trace.next ++;		// we have examined this syllable.
    trace.node = next_node;
    // we won't call pop_back() to keep the trace
    
    // create a new trace
    Trace new_trace(trace);
    new_trace.node = wl;
    Segmentation segm;
    Separator sep;
    sep.anchor = cur;
    sep.prob = prob;
    new_trace.segm.sep_list.push_back(sep);
    
    if (new_trace.next == nr_syllables) {
      vector<Separator>::iterator sepiter;
      vector<Separator> &sep_list = new_trace.segm.sep_list;
      new_trace.segm.prob = 0;
      for (sepiter = sep_list.begin(); sepiter != sep_list.end(); ++sepiter) {
	new_trace.segm.prob += sepiter->prob;
      }
      *iter++ = new_trace.segm;	// save it.
    } else
      traces.push_back(new_trace);
  }
}

void main()
{
  WFST wfst;
  WordNodePtr wl(new WordNode);

  wl->load("wordlist.wl");
  wfst.set_wordlist(wl);
  WFST::Segmentation seg;
  Sentence st("häc sinh häc sinh häc");
  st.standardize();
  st.tokenize();
  wfst.segment_best(st,seg);
}
