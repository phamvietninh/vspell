#include "wfst.h"
#include <iterator>
#include <algorithm>

using namespace std;

void Sentence::standardize()
{
}

void Sentence::tokenize()
{
}

float WordNode::get_prob() const
{
  return 0;
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

void WFST::segment_best(const string &_sent,Segmentation &seps)
{
  vector<Segmentation> segms;
  insert_iterator<vector<Segmentation> > iter(segms,segms.begin());
  segment_all(_sent,iter);

  // sort by prob
  sort(segms.begin(),segms.end(),SegmentationComparator());
}

struct Trace
{
  int next;
  WFST::Segmentation segm;
  WordNode *node;
};

template <class OutputIterator>
void WFST::segment_all(const string &_sent,OutputIterator iter)
{
  // Preprocess
  Sentence sent(_sent);
  sent.standardize();
  sent.tokenize();

  int nr_syllables = sent.get_syllable_number();
  int i;
  vector<Trace> traces;
  
  Trace trace;
  trace.next = 0;
  trace.node = wl;

  traces.push_back(trace);

  while (!traces.empty()) {
    // get one
    trace = traces.back();
    Sentence::Syllable &syll = sent[trace.next];

    WordNode *next_node = trace.node->get_next(*syll);
    if (next_node == NULL)
      traces.pop_back();	// cancel this trace

    float prob = next_node->get_prob();
    if (prob == 0)
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
}
