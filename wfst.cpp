#include "wfst.h"
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"
#include "distance.h"

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
  nodes[str] = new WordNode;
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
  int thresold = 1;
  for (iter = nodes.begin();iter != nodes.end(); ++iter) {
    int iter_size = iter->first.size();
    if (thresold > 0 &&
	(iter_size - str_len >= thresold ||
	 str_len - iter_size >= thresold))
      continue;

    ed_value = ed(iter->first.data(),iter_size,str_data,str_len);
    //cerr << boost::format("%s <> %s => %d\n") % str % iter->first % ed_value;
    if (ed_value <= thresold) {	// if (iter->first == str)
      //cerr << iter->first << "<>" << str << " => " << ed_value << endl;
      _nodes.push_back(iter->second);
    }
  }
  //cerr << str << " -> " << _nodes.size() << endl;
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
  int next,fuzzy_thresold;
  WFST::Segmentation segm;
  WordNodePtr node;
};

#define FUZZY_THRESOLD 1

template <class OutputIterator>
void WFST::segment_all(const Sentence &sent,OutputIterator iter)
{

  int nr_syllables = sent.get_syllable_number();
  int i;
  vector<Trace> traces;
  int count = 0;
  
  Trace start_trace;
  start_trace.next = 0;
  start_trace.node = wl;
  start_trace.fuzzy_thresold = 0;

  traces.push_back(start_trace);

  while (!traces.empty()) {
    // examine one
    Trace trace = traces.back();
    traces.pop_back();
    cerr << traces.size() << endl;

    // segmentation completed. throw it away
    // TUNE: do we need these? -> yes we do
    // in case of partial segmentations
    if (trace.next == nr_syllables)
      continue;

    Sentence::Syllable syll = sent[trace.next];
    //cerr << "<" << *syll << ">" << endl;

    // get next node. INFO: fuzzy match here
    vector<WordNodePtr> next_nodes;
    if (trace.fuzzy_thresold > FUZZY_THRESOLD) {
      WordNodePtr node = trace.node->get_next(*syll);
      if (node != NULL)
	next_nodes.push_back(node);
    } else
      trace.node->fuzzy_get_next(*syll,next_nodes);

    int next_nodes_size = next_nodes.size();

    if (next_nodes_size == 0)
      continue;

    if (next_nodes_size > 1)
      trace.fuzzy_thresold ++;

    vector<WordNodePtr>::iterator next_nodes_iter;
    for (next_nodes_iter = next_nodes.begin();
	 next_nodes_iter != next_nodes.end();
	 ++next_nodes_iter) {
      WordNodePtr next_node(*next_nodes_iter); 

      // start a new trace based on "trace"
      Trace new_trace = trace;
      new_trace.node = next_node;
      new_trace.next ++;
      traces.push_back(new_trace); // put it into stack
      //cerr << "new trace" << endl;


      float prob = next_node->get_prob();
      if (prob < 0)		// not the final state
	continue;		// move on

      // got a final state
      new_trace.node = wl;	// reset node
      Separator sep;		// create a separator
      sep.anchor = new_trace.next-1;
      sep.prob = prob;
      new_trace.segm.sep_list.push_back(sep); // add a new separator

      // if the new trace is completed, save it
      if (new_trace.next == nr_syllables) {
	vector<Separator>::iterator sepiter;
	vector<Separator>& seplist = new_trace.segm.sep_list;
	new_trace.segm.prob = 0;
	// calculate the prob sum
	for (sepiter = seplist.begin();
	     sepiter != seplist.end();
	     ++sepiter) {
	  new_trace.segm.prob += sepiter->prob;
	}
	*iter++ = new_trace.segm;	// save it.
	//cerr << "--> " << count++ << endl;
      } else {
	traces.push_back(new_trace);
	//cerr << "add new trace" << endl;
      }
    }
  }
}


