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

#define ED_THRESOLD 1
#define DISTANCE_THRESOLD 1	// total distances without pruning


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
    int iter_size = iter->first.size();
    if (ED_THRESOLD > 0 &&
	(iter_size - str_len >= ED_THRESOLD ||
	 str_len - iter_size >= ED_THRESOLD))
      continue;

    ed_value = ed(iter->first.data(),iter_size,str_data,str_len);
    //cerr << boost::format("%s <> %s => %d\n") % str % iter->first % ed_value;
    if (ed_value <= ED_THRESOLD) {	// if (iter->first == str)
      //cerr << iter->first << "<>" << str << " => " << ed_value << endl;
      _nodes.push_back(iter->second);
    }
#endif
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
  bool operator() (const Segmentation &seg1,const Segmentation &seg2) {
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
  vector<Segmentation>::iterator i;
  for (i = segms.begin();i != segms.end(); ++i)
    i->print(_sent);
}

// WFST (Weighted Finite State Transducer) implementation
// TUNE: think about genetic/greedy. Vietnamese is almost one-syllable words..
// we find where is likely a start of a word, then try to construct word
// and check if others are still valid words.

// the last item is always the incompleted item. We will try to complete
// a word from the item. If we reach the end of sentence, we will remove it
// from segs

template <class OutputIterator>
void WFST::segment_all(const Sentence &sent,OutputIterator iter)
{
  int nr_syllables = sent.get_syllable_count();
  vector<Segmentation> segs;	// used to store all incomplete seg
  int i;
  int count = 0;
  
  //Segmentation start_seg;
  segs.push_back(Segmentation()); // empty seg

  while (!segs.empty()) {
    // get one
    Segmentation seg = segs.back();
    segs.pop_back();
    //cerr << segs.size() << endl;

    int next_syllable = seg.items.size();
    WordNodePtr state;
    if (next_syllable == 0 ||
	seg.items[next_syllable-1].flags & SEGM_SEPARATOR)
      state = wl;
    else
      state = seg.items[next_syllable-1].state;

    // segmentation completed. throw it away
    // TUNE: do we need these? -> yes we do
    // in case of bad/incomplete segmentations
    if (next_syllable == nr_syllables)
      continue;

    const Sentence::Syllable &syll = sent[next_syllable];
    //cerr << "<" << *syll << ">" << endl;

    // get next state.
    vector<WordNodePtr> next_states;
    if (seg.distance > DISTANCE_THRESOLD) {
      // too many operations
      // use exact matching instead of fuzzy one to keep segs small
      WordNodePtr node = state->get_next(*syll);
      if (node != NULL)
	next_states.push_back(node);
    } else
      state->fuzzy_get_next(*syll,next_states);

    int next_states_size = next_states.size();

    if (next_states_size == 0)	// nothing to continue
      continue;

    if (next_states_size > 1)	// for pruning
      seg.distance ++;		// FIXME: += item.distance?

    vector<WordNodePtr>::iterator next_states_iter;
    for (next_states_iter = next_states.begin();
	 next_states_iter != next_states.end();
	 ++next_states_iter) {
      WordNodePtr next_state(*next_states_iter); 

      if (!next_state->is_next_empty()) {
	// New segmentation for longer incomplete word
	Segmentation newseg;
	newseg = seg;
	newseg.items.push_back(Segmentation::Item());
	Segmentation::Item &item = newseg.items.back();
	item.state = next_state;	// move to the next node
	item.distance = 0/*FIXME*/;
	segs.push_back(newseg);
      }

      float prob = next_state->get_prob();
      if (prob >= 0) {		// a final state
	// New segmentation for longer incomplete word
	Segmentation newseg;
	newseg = seg;
	newseg.items.push_back(Segmentation::Item());
	Segmentation::Item &item = newseg.items.back();
	item.state = next_state;	// move to the next node
	item.distance = 0/*FIXME*/;
	item.flags |= SEGM_SEPARATOR;
	if (newseg.items.size() == nr_syllables)
	  *iter++ = newseg;
	else
	  segs.push_back(newseg);
      }
    } // end for (moving to new state)
  } // end while
}


void Segmentation::print(const Sentence &st)
{
  int i,n = st.get_syllable_count();
  /*
  for (i = 0;i < n;i ++) {
    cout << *st[i] << " ";
    if (items[i].flags & SEGM_SEPARATOR)
      cout << "| ";
  }
  cout << endl;
  */
  for (i = 0;i < n;i ++) {
    cout << items[i].state->get_syllable() << " ";
    if (items[i].flags & SEGM_SEPARATOR)
      cout << "| ";
  }
  cout << endl;
}
