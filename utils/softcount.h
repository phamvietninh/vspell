#ifndef __SOFTCOUNT_H__ // -*- tab-width: 2 mode: c++ -*-
#define __SOFTCOUNT_H__

#ifndef __DICTIONARY_H__
#include "dictionary.h"
#endif

#ifndef __WORDNODE_H__
#include "wordnode.h"
#endif

#ifndef __SPELL_H__
#include "spell.h"
#endif

#ifndef __DAG_H__
#include "dag.h"
#endif
#ifndef __VECTOR__
#include <vector>
#endif

#ifndef __IOSTREAM__
#include <iostream>
#endif

#ifndef BOOST_SHARED_PTR_HPP_INCLUDED
#include <boost/shared_ptr.hpp>
#endif

namespace SoftCounter
{
	std::ostream& count_lattice(const Lattice &words,std::ostream &os,bool first_round = false);
	std::ostream& count_dag(const DAG &words,std::ostream &os,int id,bool first_round = false);
	void record2(const DAG &words,FILE *fp,int id);
	int replay2(FILE *fp_in,FILE *fp_out,int id, bool first_round = false);
	std::ostream& count_dag_fixed(const DAG &words,std::ostream &os,bool first_round = false);
};

#endif
