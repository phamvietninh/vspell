bool find_capital_words(Sentence &st,int start,int &pos,int &len);

void mark_proper_name(Sentence &st)
{
  // find all possibilities:
  // a proper name is a name consisting of words,
  // which are started with a capitalized character. 
  // e.g. Cong hoa Xa hoi Chu nghia Viet Nam
  // hmm.. these should be treated like other words for spelling checking.

  // individual name is easier: Nguyen Thai Ngoc Duy
  // these are usually started with common sirname like Nguyen, Tran, ...
  // there are 2-5 words in a name.

  // there are 2 approaches:
  // one try to find contiguous capitalized word then check for proper name
  // other try to find sirname first and find the rest.
  // which one is better?

  // if one started with a capital word, then we should check the rest with
  // our proper name list.

  // here i choose the first approach so that i can check for other proper
  // names in addition to personal names.

  int pos = 0,len = 0,npos,nlen;
  while (1) {
    if (!find_capital_words(st,pos+len,npos,nlen))
      break;

    // check for sirname
    // check for proper name
    pos = npos;
    len = nlen;
  }
}

bool find_capital_words(Sentence &st,int start,int &pos,int &len)
{
  int i, sz = st.get_syllable_count();
  bool found = false;
  string sent = st.get_sentence();
  for (i = start;i < sz;i ++) {
    if (viet_isupper(sent[st[i].start])) {
      if (!found) {
	found = true;
	pos = i;
	len = 1;
      } else {
	len ++;
      }
    } else {
      if (found)
	return true;
    }
  }
  return found;
}
