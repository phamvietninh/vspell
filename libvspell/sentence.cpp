#include "sentence.h"						// -*- tab-width: 2 -*-
#include "spell.h"
#include "tokenize.h"
#include <boost/format.hpp>
using namespace std;

void sentences_split(const string &_input,vector<string> &output)
{
	Tokens tokens;
	::tokenize(_input,tokens);

	int i,n = tokens.size();

	string str;
	bool flush = false;
	
	for (i = 0;i < n;i ++) {
		if (tokens[i].is_token) {
			int jj,nn = tokens[i].value.size();
			if (nn == 1 && strchr("?!()[];:.,",tokens[i].value[0]))
				flush = true;
			else if (flush) {
				output.push_back(str);
				str = "";
				flush = false;
				
			}
		}
		str += tokens[i].value;
	}

	if (!str.empty())
		output.push_back(str);

	/*
  // candidates are ? ! .
  // . is ambiguous
  string input = _input;

  int npos,pos = 0,len;
  bool run = true;
  bool split = false;

  while (run && !input.empty()) {
    if (split) {
      output.push_back(input.substr(0,npos+1));
      input.erase(0,npos+1);
      pos = 0;
      split = false;
    }

    npos = input.find_first_of("?!.",pos);
    if (npos == string::npos) break;

    len = input.size();

    if (!(npos + 1 < len)) break;
    if (input[npos+1] != ' ') continue;

    if (!(npos + 2 < len)) break;
    if (viet_isupper(input[npos+2])) { split = true; continue; }

    pos = npos+1;
  }

  if (!input.empty())
    output.push_back(input);
	*/
}

void Sentence::standardize()
{
}

/**
 Split punctiations off the token
 \param s input token
 \param ret a sequence of tokens

 Here is summary from SATZ tokenize.l:
 LN = letters and numbers
 LNS = letters and numbers and some: .:'$%-\/& and 0x7F
 A = apostrophe '
 SC = single characters: LN + #_;!?@*+=~|^&,:$%\ 0x7F ( ) [ ] { } < > "
 WS = white space (space tab new line)
 NL = new line
 INV = invisible (out of 32-127)

 SENTENCE_FINAL			[.?!]
 HYPHEN				[\-]
 OPEN_SINGLE_QUOTE		[\`]
 CLOSE_SINGLE_QUOTE		[\']
 RIGHT_PAREN                     [\"\)\]\}\>\']

 <p> <s> </p> </s> --> do nothing (**end**)
 SENTENCE_FINAL+RIGHT_PAREN*               .) ?) !)  ?" . ? !
 HYPHEN+                                   -  -- ---
 OPEN_SINGLE_QUOTE+                        `  `` ```
 CLOSE_SINGLE_QUOTE+                       '  '' '''
 LNS+LN                                    all end with a letter or a number
 LN+A                                      he' ll run
 SC                --> token.              c (fallback)
 WS|NL             --> ignore.             should be replaced
 WSNL+             --> token.

 Should we use flex or hand code?
 Flex is less error-prone, but it's hard to specify Vietnamese letters.
 Hand code is all right, but hard to extend later.

 Choose flex for i'm lazy ;)
*/

void Sentence::tokenize_punctuation(const string &s,vector<string> &ret)
{
	unsigned int pos = 0,start = 0;
	unsigned int npos;
	unsigned int len = s.size();
	while (start < len) {
		if (pos < len) {
			npos = s.find_first_of("!#()'\";:.,?/",pos);
			if (npos == string::npos)
				npos = len;
			pos = npos+1;
			if (npos < len) {					// TODO: some checks here
				// floating point
				if ((s[npos] == '.' || s[npos] == ',') &&
						(npos+1 < len && s[npos+1] >= '0' && s[npos+1] <= '9'))
					continue;							// skip the dot/comma

				// date
				if ((s[npos] == '/') &&
						(npos+1 < len && s[npos+1] >= '0' && s[npos+1] <= '9'))
					continue;

				// only split dot when it's in the end.
				if (s[npos] == '.' && npos+1 != len)
					continue;
			}
		} else
			npos = len;

		ret.push_back(s.substr(start,npos-start));
		if (npos < len)
			ret.push_back(s.substr(npos,1));
		start = npos+1;
	}
}

/**
 Convert a string to a sequence of token
*/

void Sentence::tokenize()
{
	Tokens tokens;
	::tokenize(sent_,tokens);

	int i,n = tokens.size();

	Syllable sy;
	sy.span = 1;
	sy.sent_ = this;
	sy.start = 0;
	
	for (i = 0;i < n;i ++) {
		if (tokens[i].is_token) {
			/*
				char *viet_token = viet_to_viscii(tokens[i].value.c_str());
				if (!viet_token) {
				sy.id = get_sarch()[tokens[i].value];
				sy.cid = get_sarch()[string("6")+tokens[i].value];
				syllables.push_back(sy);
				} else {
			*/
			const char *viet_token = tokens[i].value.c_str();
			int jj,nn = strlen(viet_token);
			for (jj = 0;jj < nn;jj ++)
				if (viet_isalpha(viet_token[jj]) || viet_isdigit(viet_token[jj])) {
					string s = viet_token;
					sy.id = get_sarch()[s];
					sy.cid = get_sarch()[get_std_syllable(s)];
					syllables.push_back(sy);
					break;
				}
			/*}*/
		}
		sy.start += tokens[i].value.size();
	}
}


/**
	 Dump a Sentence
*/

ostream& operator <<(ostream &os, const Sentence &st)
{
	int cc,i,n = st.get_syllable_count();
	for (cc = i = 0;i < n;i ++) {
		if (i) os << " ";
		os << boost::format("%s(%d-%d[%s])") % get_sarch()[st[i].id] % st[i].id % st[i].cid % get_sarch()[st[i].cid];
	}
	//os << st.prob << endl;
	
	return os;
}

/*
	std::string& Sentence::const_iterator::operator++()
	{
	}

	std::string Sentence::const_iterator::operator++(int)
	{
	}
*/


