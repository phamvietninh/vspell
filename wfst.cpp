#include "config.h"
#include "wfst.h"
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;
using namespace Dictionary;


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
    Dictionary::WordNodePtr state;
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
    vector<Dictionary::WordNodePtr> next_states;
    if (seg.distance > DISTANCE_THRESOLD) {
      // too many operations
      // use exact matching instead of fuzzy one to keep segs small
      Dictionary::WordNodePtr node = state->get_next(*syll);
      if (node != NULL)
	next_states.push_back(node);
    } else
      state->fuzzy_get_next(*syll,next_states);

    int next_states_size = next_states.size();

    if (next_states_size == 0)	// nothing to continue
      continue;

    if (next_states_size > 1)	// for pruning
      seg.distance ++;		// FIXME: += item.distance?

    vector<Dictionary::WordNodePtr>::iterator next_states_iter;
    for (next_states_iter = next_states.begin();
	 next_states_iter != next_states.end();
	 ++next_states_iter) {
      Dictionary::WordNodePtr next_state(*next_states_iter); 

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
