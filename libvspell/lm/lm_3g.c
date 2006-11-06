/* ====================================================================
 * Copyright (c) 1999-2001 Carnegie Mellon University.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */

/* 
 *
 * lm_3g.c -- Darpa Trigram LM module.
 *
 * HISTORY
 * 
 * $Log: lm_3g.c,v $
 * Revision 1.14  2005/09/01 21:09:54  dhdfu
 * Really, actually, truly consolidate byteswapping operations into
 * byteorder.h.  Where unconditional byteswapping is needed, SWAP_INT32()
 * and SWAP_INT16() are to be used.  The WORDS_BIGENDIAN macro from
 * autoconf controls the functioning of the conditional swap macros
 * (SWAP_?[LW]) whose names and semantics have been regularized.
 * Private, adhoc macros have been removed.
 *
 * Revision 1.13  2004/12/10 16:48:56  rkm
 * Added continuous density acoustic model handling
 *
 * Revision 1.12  2004/07/16 00:57:11  egouvea
 * Added Ravi's implementation of FSG support.
 *
 * Revision 1.1.1.1  2004/03/01 14:30:29  rkm
 *
 *
 * Revision 1.2  2004/02/27 16:15:13  rkm
 * Added FSG switching
 *
 * Revision 1.1.1.1  2003/12/03 20:05:04  rkm
 * Initial CVS repository
 *
 * Revision 1.11  2001/12/11 00:24:48  lenzo
 * Acknowledgement in License.
 *
 * Revision 1.10  2001/12/07 17:30:02  lenzo
 * Clean up and remove extra lines.
 *
 * Revision 1.9  2001/12/07 13:11:30  lenzo
 * Consolidate byte order code into byteorder.h.  Note that there are still
 * two "senses" of byte swapping that are confusing, and all this should
 * be cleaned up and verified.
 *
 * Revision 1.8  2001/12/07 05:09:30  lenzo
 * License.xsxc
 *
 * Revision 1.7  2001/12/07 04:27:35  lenzo
 * License cleanup.  Remove conditions on the names.  Rationale: These
 * conditions don't belong in the license itself, but in other fora that
 * offer protection for recognizeable names such as "Carnegie Mellon
 * University" and "Sphinx."  These changes also reduce interoperability
 * issues with other licenses such as the Mozilla Public License and the
 * GPL.  This update changes the top-level license files and removes the
 * old license conditions from each of the files that contained it.
 * All files in this collection fall under the copyright of the top-level
 * LICENSE file.
 *
 * Revision 1.6  2001/10/23 22:20:30  lenzo
 * Change error logging and reporting to the E_* macros that call common
 * functions.  This will obsolete logmsg.[ch] and they will be removed
 * or changed in future versions.
 *
 * Revision 1.5  2001/02/13 19:51:38  lenzo
 * *** empty log message ***
 *
 * Revision 1.4  2001/02/13 18:50:35  lenzo
 * Adding some more comments for a Solaris port.
 *
 * Revision 1.3  2000/12/12 23:01:42  lenzo
 * Rationalizing libs and names some more.  Split a/d and fe libs out.
 *
 * Revision 1.2  2000/12/05 01:45:12  lenzo
 * Restructuring, hear rationalization, warning removal, ANSIfy
 *
 * Revision 1.1.1.1  2000/01/28 22:08:51  lenzo
 * Initial import of sphinx2
 *
 * 
 * 28-Oct-98	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		Added lm3g_access_type() and necessary support.
 * 
 * 15-Oct-98	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		Bugfix: inclass_ugscore[lw3] changed to inclass_ugscore[w3] in
 * 		lm3g_tg_score().  (Thanks to dbansal@cs.)
 * 
 * 15-Jul-98	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		Corrected references to unigram_t.wid to unigram_t.mapid.
 * 
 * 14-Apr-98	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		Added lm3g_n_lm() and lm3g_index2name().
 * 
 * 03-Apr-97	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		Added lm3g_raw_score() and lm_t.invlw.
 * 		Changed a number of function names from lm_... to lm3g_...
 * 
 * 09-Jan-97	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		BUGFIX: Added check for lmp->unigrams[i].wid in lm_set_current().
 * 
 * 06-Dec-95	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		Changed function name lmname_to_lm() to lm_name2lm().
 * 
 * 06-Dec-95	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		Changed function name get_current_lm to lm_get_current.
 * 		Changed check for already existing word in lm_add_word, and added
 * 		condition to updating dictwid_map.
 * 
 * 01-Jul-95	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		Removed LM cache and replaced with find_bg and find_tg within the main
 * 		bigrams and trigram structures.  No loss of speed and uses less memory.
 * 
 * 24-Jun-95	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		Fixed a number of memory leaks while deleting an LM.  Added the global
 * 		dictwid_map, and allocated it once and never freed.  Made sure lm_cache
 * 		is created only once.
 * 
 * 14-Jun-95	M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * 		Modified lm_read to return 0 on success, and to delete any existing LM
 * 		with the new LM name (instead of reporting error and exiting).
 * 		Added backslash option in building filenames (for PC compatibility).
 *
 * Revision 8.9  94/10/11  12:36:28  rkm
 * Changed lm_tg_score to call lm_bg_score if no trigrams present or
 * the first word is invalid.
 * 
 * Revision 8.8  94/07/29  11:54:23  rkm
 * Renamed lmSetParameters to lm_set_param and moved it into lm_add().
 * Added functions lm_init_oov() to create initial list of OOVs,
 * and lm_add_word() to add new OOV at run time.
 * 
 * Revision 8.7  94/05/19  14:19:59  rkm
 * Rewrote LM cache code for greater efficiency.
 * 
 * Revision 8.6  94/05/10  10:47:58  rkm
 * Added lm_add() and lm_set_param() functions, for dynamically adding a new
 * in-memory LM to the set of available LMs.
 * 
 * Revision 8.5  94/04/22  13:53:27  rkm
 * Added query_lm_cache_lines() to allow run-time spec of #cache lines.
 * 
 * Revision 8.4  94/04/14  15:08:46  rkm
 * Added function lm_delete() to delete a named LM and reclaim space.
 * 
 * Revision 8.3  94/04/14  14:40:27  rkm
 * Minor changes.
 * 
 * Revision 8.1  94/02/15  15:09:22  rkm
 * Derived from v7.  Includes multiple LMs for grammar switching.
 * 
 * Revision 6.13  94/02/11  13:14:45  rkm
 * Added bigram and trigram multi-line caches, and functions, for v7.
 * Replaced sequential search in lm3g_wstr2wid() with hash_lookup().
 * 
 * Revision 6.12  94/01/07  10:56:16  rkm
 * Corrected bug relating to input file format.
 * 
 * Revision 6.11  93/12/17  13:14:52  rkm
 * *** empty log message ***
 * 
 * Revision 6.10  93/12/03  17:09:59  rkm
 * Added ability to handle bigram-only dump files.
 * Added <s> </s> bigram -> MIN_PROB.
 * Added timestamp to dump files.
 * 
 * Revision 6.9  93/12/01  12:29:55  rkm
 * Added ability to handle LM files containing only bigrams.
 * Excluded start_sym from interpolation of unigram prob with uniform prob.
 * 
 * 
 * 93/10/21 rkm@cs.cmu.edu
 * Added <c.h>
 * 
 * Revision 6.6  93/10/19  18:58:10  rkm
 * Added code to change bigram-prob(<s>,<s>) to very low value.  The
 * Darpa LM file contains a spurious value to be ignored.
 * Fixed bug that dumps one trigram entry too many.
 * 
 * Revision 6.5  93/10/15  15:00:14  rkm
 * 
 * Revision 6.4  93/10/13  16:56:04  rkm
 * Added LM cache line stats.
 * Added bg_only option for lm_read parameter list
 * (but not yet implemented).
 * Changed proc name ilm_LOG_prob_of to lm3g_prob, to avoid conflict
 * with Roni's ILM function of the same name.
 * 
 * Revision 6.3  93/10/09  17:01:55  rkm
 * M K Ravishankar (rkm@cs) at Carnegie Mellon
 * Cleaned up handling precompiled binary 3g LM file,
 * Added SWAP option for HP platforms.
 * 
 * Revision 6.2  93/10/06  11:08:15  rkm
 * M K Ravishankar (rkm@cs) at Carnegie Mellon University
 * Darpa Trigram LM module.  Created.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define QUIT(x)		{fflush(stdout); fprintf x; exit(-1);}

#include "s2types.h"
#include "CM_macros.h"
#include "assert.h"
#include "strfuncs.h"
#include "linklist.h"
#include "list.h"
#include "hash.h"
#include "err.h"
#include "lm_3g.h"
#include "log.h"

#define NO_WORD	0

#define UG_MAPID(m,u)		((m)->unigrams[u].mapid)
#define UG_PROB_F(m,u)		((m)->unigrams[u].prob1.f)
#define UG_BO_WT_F(m,u)		((m)->unigrams[u].bo_wt1.f)
#define UG_PROB_L(m,u)		((m)->unigrams[u].prob1.l)
#define UG_BO_WT_L(m,u)		((m)->unigrams[u].bo_wt1.l)
#define FIRST_BG(m,u)		((m)->unigrams[u].bigrams)
#define LAST_BG(m,u)		(FIRST_BG((m),(u)+1)-1)

#define BG_WID(m,b)		((m)->bigrams[b].wid)
#define BG_PROB_F(m,b)		((m)->prob2[(m)->bigrams[b].prob2].f)
#define BG_BO_WT_F(m,b)		((m)->bo_wt2[(m)->bigrams[b].bo_wt2].f)
#define BG_PROB_L(m,b)		((m)->prob2[(m)->bigrams[b].prob2].l)
#define BG_BO_WT_L(m,b)		((m)->bo_wt2[(m)->bigrams[b].bo_wt2].l)
#define TSEG_BASE(m,b)		((m)->tseg_base[(b)>>LOG_BG_SEG_SZ])
#define FIRST_TG(m,b)		(TSEG_BASE((m),(b))+((m)->bigrams[b].trigrams))
#define LAST_TG(m,b)		(FIRST_TG((m),(b)+1)-1)

#define TG_WID(m,t)		((m)->trigrams[t].wid)
#define TG_PROB_F(m,t)		((m)->prob3[(m)->trigrams[t].prob3].f)
#define TG_PROB_L(m,t)		((m)->prob3[(m)->trigrams[t].prob3].l)

/* FIXME: put this in a header file */
extern void quit (int status, char const *fmt, ...);

static char const *start_sym = "<s>";
static char const *end_sym = "</s>";



#define MIN_PROB_F		-99.0

#define MAX_SORTED_ENTRIES	65534

/* Base values for ranges of unigram_t.mapid */
#define LM_DICTWID_BASE		0		/* Do not change this */
#define LM_CLASSID_BASE		0x01000000	/* Interpreted as LMclass ID */
#define LM_DICTWID_BADMAP	-16000		/* An illegal mapping */



/*
 * Initialize sorted list with the 0-th entry = MIN_PROB_F, which may be needed
 * to replace spurious values in the Darpa LM file.
 */
static void init_sorted_list (sorted_list_t *l)
{
    l->list =
	(sorted_entry_t *) CM_calloc (MAX_SORTED_ENTRIES, sizeof (sorted_entry_t));
    l->list[0].val.f = MIN_PROB_F;
    l->list[0].lower = 0;
    l->list[0].higher = 0;
    l->free = 1;
}

static void free_sorted_list (sorted_list_t *l)
{
    free (l->list);
}

static log_t *vals_in_sorted_list (sorted_list_t *l)
{
    log_t *vals;
    int32 i;
    
    vals = (log_t *) CM_calloc (l->free, sizeof (log_t));
    for (i = 0; i < l->free; i++)
	vals[i].f = l->list[i].val.f;
    return (vals);
}

static int32 sorted_id (sorted_list_t *l, float *val)
{
    int32 i = 0;
    
    for (;;) {
	if (*val == l->list[i].val.f)
	    return (i);
	if (*val < l->list[i].val.f) {
	    if (l->list[i].lower == 0) {
		if (l->free >= MAX_SORTED_ENTRIES)
		    E_FATAL("sorted list overflow\n");
		
		l->list[i].lower = l->free;
		(l->free)++;
		i = l->list[i].lower;
		l->list[i].val.f = *val;
		return (i);
	    } else
		i = l->list[i].lower;
	} else {
	    if (l->list[i].higher == 0) {
		if (l->free >= MAX_SORTED_ENTRIES)
		    E_FATAL("sorted list overflow\n");
		
		l->list[i].higher = l->free;
		(l->free)++;
		i = l->list[i].higher;
		l->list[i].val.f = *val;
		return (i);
	    } else
		i = l->list[i].higher;
	}
    }
}

/*
 * allocate, initialize and return pointer to an array of unigram entries.
 */
static unigram_t *NewUnigramTable (int32 n_ug)
{
    unigram_t *table;
    int32 i;

    table = (unigram_t *) CM_calloc (n_ug, sizeof (unigram_t));
    for (i = 0; i < n_ug; i++) {
	table[i].mapid = NO_WORD;
	table[i].prob1.f = -99.0;
	table[i].bo_wt1.f = -99.0;
    }
    return table;
}

/*
 * returns a pointer to a new language model record.  The size is passed in
 * as a parameter.
 */
lm_t *
NewModel (n_ug, n_bg, n_tg)
    int32 n_ug;
    int32 n_bg;
    int32 n_tg;
{
    lm_t *model;

    model = (lm_t *) CM_calloc (1, sizeof (lm_t));

    /*
     * Allocate one extra unigram and bigram entry: sentinels to terminate
     * followers (bigrams and trigrams, respectively) of previous entry.
     */
    model->unigrams	= NewUnigramTable (n_ug+1);
    model->bigrams	= (bigram_t *) CM_calloc (n_bg+1, sizeof (bigram_t));
    if (n_tg > 0)
	model->trigrams	= (trigram_t *) CM_calloc (n_tg, sizeof (trigram_t));

    if (n_tg > 0) {
	model->tseg_base = (int32 *) CM_calloc ((n_bg+1)/BG_SEG_SZ+1, sizeof (int32));
#if 0
	E_INFO("%8d = tseg_base entries allocated\n",
	       (n_bg+1)/BG_SEG_SZ+1);
#endif
    }

    model->max_ucount = model->ucount = n_ug;
    model->bcount = n_bg;
    model->tcount = n_tg;
    
    model->HT.size = 0;
    model->HT.inuse = 0;
    model->HT.tab = NULL;

    return model;
}

int32 lm3g_wstr2wid (lm_t *model, const char *w)
{
    caddr_t val;
    
    if (hash_lookup (&(model->HT), w, &val) != 0)
	return NO_WORD;
    return ((int32) val);
}

/*
 * Read and return #unigrams, #bigrams, #trigrams as stated in input file.
 */
static void ReadNgramCounts (FILE *fp,
			     int32 *n_ug, int32 *n_bg, int32 *n_tg)
{
    char string[256];
    int32 ngram, ngram_cnt;
    
    /* skip file until past the '\data\' marker */
    do
	fgets (string, sizeof (string), fp);
    while ( (strcmp (string, "\\data\\\n") != 0) && (! feof (fp)) );

    if (strcmp (string, "\\data\\\n") != 0)
	E_FATAL("No \\data\\ mark in LM file\n");

    *n_ug = *n_bg = *n_tg = 0;
    while (fgets (string, sizeof (string), fp) != NULL) {
	if (sscanf (string, "ngram %d=%d", &ngram, &ngram_cnt) != 2)
	    break;
	switch (ngram) {
	case 1: *n_ug = ngram_cnt;
	    break;
	case 2: *n_bg = ngram_cnt;
	    break;
	case 3: *n_tg = ngram_cnt;
	    break;
	default:
	    E_FATAL("Unknown ngram (%d)\n", ngram);
	    break;
	}
    }
    
    /* Position file to just after the unigrams header '\1-grams:\' */
    while ( (strcmp (string, "\\1-grams:\n") != 0) && (! feof (fp)) )
	fgets (string, sizeof (string), fp);
    
    /* Check counts;  NOTE: #trigrams *CAN* be 0 */
    if ((*n_ug <= 0) || (*n_bg <= 0) || (*n_tg < 0))
	E_FATAL("Bad or missing ngram count\n");
}

/*
 * Read in the unigrams from given file into the LM structure model.  On
 * entry to this procedure, the file pointer is positioned just after the
 * header line '\1-grams:'.
 */
static void ReadUnigrams (FILE *fp, lm_t *model)
{
    char string[256];
    char name[128];
    int32 wcnt;
    float p1, bo_wt;
    
    E_INFO ("Reading unigrams\n");

    wcnt = 0;
    while ((fgets (string, sizeof(string), fp) != NULL) &&
	   (strcmp (string, "\\2-grams:\n") != 0))
    {
	if (sscanf (string, "%f %s %f", &p1, name, &bo_wt) != 3) {
	    if (string[0] != '\n')
		E_WARN ("Format error; unigram ignored:%s", string);
	    continue;
	}
	
	if (wcnt >= model->ucount)
	    E_FATAL("Too many unigrams\n");

	/* Associate name with word id */
	model->word_str[wcnt] = (char *) salloc (name);
	hash_add (&(model->HT), model->word_str[wcnt], (caddr_t) (wcnt+1));
	model->unigrams[wcnt].prob1.f = p1;
	model->unigrams[wcnt].bo_wt1.f = bo_wt;

	model->unigrams[wcnt].mapid = wcnt;
	wcnt++;
    }

    if (model->ucount != wcnt) {
	E_WARN ("lm_t.ucount(%d) != #unigrams read(%d)\n",
		model->ucount, wcnt);
	model->ucount = wcnt;
    }
}

/*
 * Read bigrams from given file into given model structure.  File may be arpabo
 * or arpabo-id format, depending on idfmt = 0 or 1.
 */
static void ReadBigrams (FILE *fp, lm_t *model, int32 idfmt)
{
    char string[1024], word1[256], word2[256];
    int32 w1, w2, prev_w1, bgcount, p;
    bigram_t *bgptr;
    float p2, bo_wt;
    int32 n_fld, n;
    
    E_INFO ("Reading bigrams\n");
    
    bgcount = 0;
    bgptr = model->bigrams;
    prev_w1 = -1;
    n_fld = (model->tcount > 0) ? 4 : 3;

    bo_wt = 0.0;
    while (fgets (string, sizeof(string), fp) != NULL) {
	if (! idfmt)
	    n = sscanf (string, "%f %s %s %f", &p2, word1, word2, &bo_wt);
	else
	    n = sscanf (string, "%f %d %d %f", &p2, &w1, &w2, &bo_wt);
	if (n < n_fld) {
	    if (string[0] != '\n')
		break;
	    continue;
	}

	if (! idfmt) {
	    if ((w1 = lm3g_wstr2wid (model, word1)) == NO_WORD)
		E_FATAL("Unknown word: %s\n", word1);
	    w1 --; // hash value is 1-base
	    if ((w2 = lm3g_wstr2wid (model, word2)) == NO_WORD)
		E_FATAL("Unknown word: %s\n", word2);
	    w2 --; // hash value is 1-base
	} else {
	    if ((w1 >= model->ucount) || (w2 >= model->ucount) || (w1 < 0) || (w2 < 0))
		E_FATAL("Bad bigram: %s", string);
	}
	
	/* HACK!! to quantize probs to 4 decimal digits */
	p = p2*10000;
	p2 = p*0.0001;
	p = bo_wt*10000;
	bo_wt = p*0.0001;

	if (bgcount >= model->bcount)
	    E_FATAL("Too many bigrams\n");
	
	bgptr->wid = w2;
	bgptr->prob2 = sorted_id (&model->sorted_prob2, &p2);
	if (model->tcount > 0)
	    bgptr->bo_wt2 = sorted_id (&model->sorted_bo_wt2, &bo_wt);

	if (w1 != prev_w1) {
	    if (w1 < prev_w1)
	      E_FATAL("Bigrams not in unigram order\n");
	    
	    for (prev_w1++; prev_w1 <= w1; prev_w1++)
		model->unigrams[prev_w1].bigrams = bgcount;
	    prev_w1 = w1;
	}
	
	bgcount++;
	bgptr++;

	if ((bgcount & 0x0000ffff) == 0) {
	    E_INFOCONT (".");
	}
    }
    if ((strcmp (string, "\\end\\\n") != 0) && (strcmp (string, "\\3-grams:\n") != 0))
	E_FATAL("Bad bigram: %s\n", string);
    
    for (prev_w1++; prev_w1 <= model->ucount; prev_w1++)
	model->unigrams[prev_w1].bigrams = bgcount;
}

/*
 * Very similar to ReadBigrams.
 */
static void ReadTrigrams (FILE *fp, lm_t *model, int32 idfmt)
{
    char string[1024], word1[256], word2[256], word3[256];
    int32 i, n, w1, w2, w3, prev_w1, prev_w2, tgcount, prev_bg, bg, endbg, p;
    int32 seg, prev_seg, prev_seg_lastbg;
    trigram_t *tgptr;
    bigram_t *bgptr;
    float p3;
    
    E_INFO ("Reading trigrams\n");
    
    tgcount = 0;
    tgptr = model->trigrams;
    prev_w1 = -1;
    prev_w2 = -1;
    prev_bg = -1;
    prev_seg = -1;

    while (fgets (string, sizeof(string), fp) != NULL) {
	if (! idfmt)
	    n = sscanf (string, "%f %s %s %s", &p3, word1, word2, word3);
	else
	    n = sscanf (string, "%f %d %d %d", &p3, &w1, &w2, &w3);
	if (n != 4) {
	    if (string[0] != '\n')
		break;
	    continue;
	}

	if (! idfmt) {
	    if ((w1 = lm3g_wstr2wid (model, word1)) == NO_WORD)
		E_FATAL("Unknown word: %s\n", word1);
	    w1 --; // hash value is 1-base
	    if ((w2 = lm3g_wstr2wid (model, word2)) == NO_WORD)
		E_FATAL("Unknown word: %s\n", word2);
	    w2 --; // hash value is 1-base
	    if ((w3 = lm3g_wstr2wid (model, word3)) == NO_WORD)
		E_FATAL("Unknown word: %s\n", word3);
	    w3 --; // hash value is 1-base
	} else {
	    if ((w1 >= model->ucount) || (w2 >= model->ucount) || (w3 >= model->ucount) ||
		    (w1 < 0) || (w2 < 0) || (w3 < 0))
		E_FATAL("Bad trigram: %s", string);
	}
	
	/* HACK!! to quantize probs to 4 decimal digits */
	p = p3*10000;
	p3 = p*0.0001;

	if (tgcount >= model->tcount)
	  E_FATAL("Too many trigrams\n");
	
	tgptr->wid = w3;
	tgptr->prob3 = sorted_id (&model->sorted_prob3, &p3);

	if ((w1 != prev_w1) || (w2 != prev_w2)) {
	    /* Trigram for a new bigram; update tg info for all previous bigrams */
	    if ((w1 < prev_w1) || ((w1 == prev_w1) && (w2 < prev_w2)))
	      E_FATAL("Trigrams not in bigram order\n");
	    
	    bg = (w1 != prev_w1) ? model->unigrams[w1].bigrams : prev_bg+1;
	    endbg = model->unigrams[w1+1].bigrams;
	    bgptr = model->bigrams + bg;
	    for (; (bg < endbg) && (bgptr->wid != w2); bg++, bgptr++);
	    if (bg >= endbg)
		E_FATAL("Missing bigram for trigram: %s", string);

	    /* bg = bigram entry index for <w1,w2>.  Update tseg_base */
	    seg = bg >> LOG_BG_SEG_SZ;
	    for (i = prev_seg+1; i <= seg; i++)
		model->tseg_base[i] = tgcount;

	    /* Update trigrams pointers for all bigrams until bg */
	    if (prev_seg < seg) {
		int32 tgoff = 0;

		if (prev_seg >= 0) {
		    tgoff = tgcount - model->tseg_base[prev_seg];
		    if (tgoff > 65535)
		      E_FATAL("Offset from tseg_base > 65535\n");
		}
		
		prev_seg_lastbg = ((prev_seg+1) << LOG_BG_SEG_SZ) - 1;
		bgptr = model->bigrams + prev_bg;
		for (++prev_bg, ++bgptr; prev_bg <= prev_seg_lastbg; prev_bg++, bgptr++)
		    bgptr->trigrams = tgoff;
		
		for (; prev_bg <= bg; prev_bg++, bgptr++)
		    bgptr->trigrams = 0;
	    } else {
		int32 tgoff;

		tgoff = tgcount - model->tseg_base[prev_seg];
		if (tgoff > 65535)
		  E_FATAL("Offset from tseg_base > 65535\n");
		
		bgptr = model->bigrams + prev_bg;
		for (++prev_bg, ++bgptr; prev_bg <= bg; prev_bg++, bgptr++)
		    bgptr->trigrams = tgoff;
	    }
	    
	    prev_w1 = w1;
	    prev_w2 = w2;
	    prev_bg = bg;
	    prev_seg = seg;
	}
	
	tgcount++;
	tgptr++;

	if ((tgcount & 0x0000ffff) == 0) {
	    E_INFOCONT (".");
	}
    }
    if (strcmp (string, "\\end\\\n") != 0)
	E_FATAL("Bad trigram: %s\n", string);
    
    for (prev_bg++; prev_bg <= model->bcount; prev_bg++) {
	if ((prev_bg & (BG_SEG_SZ-1)) == 0)
	    model->tseg_base[prev_bg >> LOG_BG_SEG_SZ] = tgcount;
	if ((tgcount - model->tseg_base[prev_bg >> LOG_BG_SEG_SZ]) > 65535)
	    E_FATAL("Offset from tseg_base > 65535\n");
	model->bigrams[prev_bg].trigrams =
	    tgcount - model->tseg_base[prev_bg >> LOG_BG_SEG_SZ];
    }
}

static FILE *lm_file_open (char const *filename, int32 usepipe)
{
    char command[1024];
    FILE *fp;
    
    if (usepipe) {
#ifdef WIN32
	sprintf (command, "D:\\compress\\gzip.exe -d -c %s", filename);
	if ((fp = _popen (command, "r")) == NULL)
	    E_FATAL("Cannot popen %s\n", command);
#else
	sprintf (command, "zcat %s", filename);
	if ((fp = popen (command, "r")) == NULL)
	    E_FATAL("Cannot popen %s\n", command);
#endif
    }
    else {
        fp = CM_fopen (filename, "r");
    }
    return (fp);
}

/*
 * Read in a trigram language model from the given file.  The LM tokens can be word
 * classes.  However a given actual word can belong to AT MOST ONE of the LM classes
 * used by this LM.
 */
lm_t* lm3g_read (char const *filename)
{
    lm_t *model;
    FILE *fp = NULL;
    int32 usingPipe = FALSE;
    int32 n_unigram;
    int32 n_bigram;
    int32 n_trigram;
    int32 dict_size;
    int32 i, j, k, last_bg, last_tg;
    struct stat statbuf;
    int32 idfmt;
    
    E_INFO ("Reading LM file %s\n", filename);
    
    /* Check if a compressed file */
    k = strlen(filename);
#ifdef WIN32
    usingPipe = (k > 3) &&
	((strcmp (filename+k-3, ".gz") == 0) || (strcmp (filename+k-3, ".GZ") == 0));
#else
    usingPipe = (k > 2) &&
	((strcmp (filename+k-2, ".Z") == 0) || (strcmp (filename+k-2, ".z") == 0));
#endif
    /* Check if an .arpabo-id format file; More HACK!! Hardwired check for -id */
    if (usingPipe)
	k -= 2;
    idfmt = ((k > 3) && (strncmp (filename+k-3, "-id", 3) == 0));

    fp = lm_file_open (filename, usingPipe);
    if (stat(filename, &statbuf) < 0)
      E_FATAL("stat(%s) failed\n", filename);

    /* Read #unigrams, #bigrams, #trigrams from file */
    ReadNgramCounts (fp, &n_unigram, &n_bigram, &n_trigram);
    E_INFO ("ngrams 1=%d, 2=%d, 3=%d\n", n_unigram, n_bigram, n_trigram);
    
    /* Determine dictionary size (for dict-wid -> LM-wid map) */
    dict_size = n_unigram;

    if (dict_size >= 65535)
	E_FATAL("#dict-words(%d) > 65534\n", dict_size);

    /* Allocate space for LM, including initial OOVs and placeholders; initialize it */
    model = NewModel (n_unigram, n_bigram, n_trigram);
    model->word_str = (char **) CM_calloc (n_unigram, sizeof (char *));
    model->n_word_str = n_unigram;
    
    /* Load the precompiled binary dump form of the Darpa LM file if it exists */
    if (1) {
	ReadUnigrams (fp, model);
	E_INFO("%8d = #unigrams created\n", model->ucount);
	
	init_sorted_list (&model->sorted_prob2);
	if (model->tcount > 0)
	    init_sorted_list (&model->sorted_bo_wt2);
	
	ReadBigrams (fp, model, idfmt);
	
	model->bcount = FIRST_BG(model,model->ucount);
	model->n_prob2 = model->sorted_prob2.free;
	model->prob2  = vals_in_sorted_list (&model->sorted_prob2);
	free_sorted_list (&model->sorted_prob2);
	E_INFO("\n%8d = #bigrams created\n", model->bcount);
	E_INFO("%8d = #prob2 entries\n", model->n_prob2);
	
	if (model->tcount > 0) {
	    /* Create trigram bo-wts array */
	    model->n_bo_wt2 = model->sorted_bo_wt2.free;
	    model->bo_wt2 = vals_in_sorted_list (&model->sorted_bo_wt2);
	    free_sorted_list (&model->sorted_bo_wt2);
	    E_INFO("%8d = #bo_wt2 entries\n", model->n_bo_wt2);
	    
	    init_sorted_list (&model->sorted_prob3);
	    
	    ReadTrigrams (fp, model, idfmt);
	    
	    model->tcount = FIRST_TG(model,model->bcount);
	    model->n_prob3 = model->sorted_prob3.free;
	    model->prob3  = vals_in_sorted_list (&model->sorted_prob3);
	    E_INFO("\n%8d = #trigrams created\n", model->tcount);
	    E_INFO("%8d = #prob3 entries\n", model->n_prob3);
	    
	    free_sorted_list (&model->sorted_prob3);
	}
    }

    fclose (fp);

    /*
     * Discourage expansion of end_sym and transition to start_sym.  (The given
     * Darpa LM may contain some spurious values that don't reflect these
     * requirements.)
     */
    /* bo_wt(</s>) = MIN_PROB_F */
    for (i = 0; (i < model->ucount) && (strcmp (model->word_str[i], end_sym) != 0); i++);
    E_INFO("bo_wt(%s) changed from %.4f to %.4f\n",
	   model->word_str[i], model->unigrams[i].bo_wt1.f, MIN_PROB_F);
    model->unigrams[i].bo_wt1.f = MIN_PROB_F;

    /* unigram prob(<s>) = MIN_PROB_F */
    for (i = 0; (i < model->ucount) && (strcmp (model->word_str[i], start_sym) != 0); i++);
    E_INFO("prob(%s) changed from %.4f to %.4f\n",
	   model->word_str[i], model->unigrams[i].prob1.f, MIN_PROB_F);
    model->unigrams[i].prob1.f = MIN_PROB_F;

    /* bigram prob(<s>,<s>) = MIN_PROB_F (if bigram exists) */
    j = FIRST_BG(model,i);
    last_bg = LAST_BG(model,i);
    for (; (j<=last_bg) && (strcmp(model->word_str[BG_WID(model,j)],start_sym)!=0); j++);
    if (j <= last_bg) {
	E_INFO("prob(%s,%s) changed from %.4f to %.4f\n",
	       model->word_str[i], model->word_str[BG_WID(model,j)],
	       model->prob2[model->bigrams[j].prob2].f,
	       model->prob2[0].f);
	model->bigrams[j].prob2 = 0;

	if (model->tcount > 0) {
	    /* trigram prob(<s>,<s>,<s>) = MIN_PROB_F (if trigram exists) */
	    k = FIRST_TG(model,j);
	    last_tg = LAST_TG(model,j);
	    for (; k <= last_tg; k++) {
		if (strcmp (model->word_str[TG_WID(model,k)], start_sym) == 0)
		    break;
	    }
	    if (k <= last_tg) {
		E_INFO("prob(%s,%s,%s) changed from %.4f to %.4f\n",
		       model->word_str[i], model->word_str[BG_WID(model,j)], model->word_str[TG_WID(model,k)], 
		       model->prob3[model->trigrams[k].prob3].f,
		       model->prob3[0].f);
		model->trigrams[k].prob3 = 0;
	    }
	}
    }

    /* bigram prob(<s>,</s>) = MIN_PROB_F (if bigram exists) */
    j = FIRST_BG(model,i);
    last_bg = LAST_BG(model,i);
    for (; (j<=last_bg) && (strcmp(model->word_str[BG_WID(model,j)],end_sym)!=0); j++);
    if (j <= last_bg) {
	E_INFO("prob(%s,%s) changed from %.4f to %.4f\n",
	       model->word_str[i], model->word_str[BG_WID(model,j)],
	       model->prob2[model->bigrams[j].prob2].f,
	       model->prob2[0].f);
	model->bigrams[j].prob2 = 0;
    }
    
    return model;
}

void lm3g_free (lm_t *model)
{
    int32 i,u;
    tginfo_t *tginfo, *next_tginfo;
    
    hash_free (&(model->HT));
    for (i = 0; i < model->ucount; i++)
	free (model->word_str[i]);
    free (model->word_str);
    
    free (model->unigrams);
    free (model->bigrams);
    free (model->prob2);
    if (model->tcount > 0) {
	free (model->trigrams);
	free (model->tseg_base);
	free (model->bo_wt2);
	free (model->prob3);
    }
    if (model->HT.tab != NULL)
	hash_free (&model->HT);
    
    for (u = 0; u < model->max_ucount; u++)
	for (tginfo = model->tginfo[u]; tginfo; tginfo = next_tginfo) {
	    next_tginfo = tginfo->next;
	    listelem_free ((void *)tginfo, sizeof(tginfo_t));
	}
    free (model->tginfo);

    free (model);
}

void lmSetStartSym (char const *sym)
/*----------------------------*
 * Description - reconfigure the start symbol
 */
{
    start_sym = (char *) salloc(sym);
}

void lmSetEndSym (char const *sym)
/*----------------------------*
 * Description - reconfigure the end symbol
 */
{
    end_sym = (char *) salloc(sym);
}

#ifdef NO_DICT
main (argc, argv)
    int32 argc;
    char *argv[];
{
    lm_t *model;
    int32 i, n, score;
    float lw, uw, wip;
    char wd[3][100], line[1000];
    int32 wid[3];
    int32 unkwid;
    
    if (argc < 4)
	E_FATAL("Usage: %s <lw> <uw> <wip> [LM-file]\n", argv[0]);
    if (sscanf(argv[1], "%f", &lw) != 1)
	E_FATAL("Usage: %s <lw> <uw> <wip> [LM-file]\n", argv[0]);
    if (sscanf(argv[2], "%f", &uw) != 1)
	E_FATAL("Usage: %s <lw> <uw> <wip> [LM-file]\n", argv[0]);
    if (sscanf(argv[3], "%f", &wip) != 1)
	E_FATAL("Usage: %s <lw> <uw> <wip> [LM-file]\n", argv[0]);
    if (argc == 5)
	lm_read(argv[4], lw, uw, wip, 1.0);
    else
	lm_read("/net/alf8/cdrom/wsj1/grammar/tgboc20o.nvp", lw, uw, wip, 1.0);
	
    lmSetParameters (model, 0);
    
    unkwid = lm3g_wstr2wid (model, "<UNK>");
    unkwid --; // hash value is 1-base
    for (;;) {
	printf ("Enter 1, 2, or 3 words: ");
	fgets (line, sizeof(line), stdin);
	if (((n = sscanf (line, "%s %s %s", wd[0], wd[1], wd[2])) < 1) || (n > 3))
	    break;
	for (i = 0; i < n; i++)
	    if ((wid[i] = lm3g_wstr2wid (model, wd[i])) == NO_WORD) {
		printf ("  %s -> <UNK>\n", wd[i]);
		wid[i] = unkwid;
	    }
	    else
	        wid[i] --;
	
	switch (n) {
	case 1:
	    score = unigram_score (wid[0]);
	    printf ("unigram_score(%d) = %d\n",
		    wid[0], score);
	    break;
	case 2:
	    score = bigram_score (wid[0], wid[1]);
	    printf ("bigram_score(%d, %d) = %d\n",
		    wid[0], wid[1], score);
	    break;
	case 3:
	    score = trigram_score (wid[0], wid[1], wid[2]);
	    printf ("trigram_score(%d, %d, %d) = %d\n",
		    wid[0], wid[1], wid[2], score);
	    break;
	default:
	    break;
	}
    }
}
#endif

#define BINARY_SEARCH_THRESH	16

int32 lm3g_ug_score (lm_t *lm, int32 wid)
{
    if (wid >= lm->ucount)
	E_FATAL("dictwid[%d] not in LM\n", wid);

    return (lm->unigrams[wid].prob1.l);
}

/* Locate a specific bigram within a bigram list */
static int32 find_bg (bigram_t *bg, int32 n, int32 w)
{
    int32 i, b, e;
    
    /* Binary search until segment size < threshold */
    b = 0;
    e = n;
    while (e-b > BINARY_SEARCH_THRESH) {
	i = (b+e)>>1;
	if (bg[i].wid < w)
	    b = i+1;
	else if (bg[i].wid > w)
	    e = i;
	else
	    return i;
    }

    /* Linear search within narrowed segment */
    for (i = b; (i < e) && (bg[i].wid != w); i++);
    return ((i < e) ? i : -1);
}

/* w1, w2 are dictionary (base-)word ids */
int32 lm3g_bg_score (lm_t *lm, int32 w1, int32 w2)
{
    int32 i, n, b, score;
    bigram_t *bg;
    
    /* lm->n_bg_score++; */
    
    if (w1 >= lm->ucount)
	E_FATAL("dictwid[%d] not in LM\n", w1);
    if (w2 >= lm->ucount)
	E_FATAL("dictwid[%d] not in LM\n", w2);
    
    b = FIRST_BG(lm, w1);
    n = FIRST_BG(lm, w1+1) - b;
    bg = lm->bigrams + b;
    
    if ((i = find_bg (bg, n, w2)) >= 0) {
	score = lm->prob2[bg[i].prob2].l;
    } else {
	/* lm->n_bg_bo++; */
	score = lm->unigrams[w1].bo_wt1.l + lm->unigrams[w2].prob1.l;
    }

    return (score);
}

static void load_tginfo (lm_t *lm, int32 lw1, int32 lw2)
{
    int32 i, n, b, t;
    bigram_t *bg;
    tginfo_t *tginfo;
    
    /* First allocate space for tg information for bg lw1,lw2 */
    tginfo = (tginfo_t *) listelem_alloc (sizeof(tginfo_t));
    tginfo->w1 = lw1;
    tginfo->tg = NULL;
    tginfo->next = lm->tginfo[lw2];
    lm->tginfo[lw2] = tginfo;
    
    /* Locate bigram lw1,lw2 */

    b = lm->unigrams[lw1].bigrams;
    n = lm->unigrams[lw1+1].bigrams - b;
    bg = lm->bigrams + b;
    
    if ((n > 0) && ((i = find_bg (bg, n, lw2)) >= 0)) {
	tginfo->bowt = lm->bo_wt2[bg[i].bo_wt2].l;
	
	/* Find t = Absolute first trigram index for bigram lw1,lw2 */
	b += i;			/* b = Absolute index of bigram lw1,lw2 on disk */
	t = FIRST_TG(lm, b);

	tginfo->tg = lm->trigrams + t;
	
	/* Find #tg for bigram w1,w2 */
	tginfo->n_tg = FIRST_TG(lm, b+1) - t;
    } else {			/* No bigram w1,w2 */
	tginfo->bowt = 0;
	tginfo->n_tg = 0;
    }
}

/* Similar to find_bg */
static int32 find_tg (trigram_t *tg, int32 n, int32 w)
{
    int32 i, b, e;
    
    b = 0;
    e = n;
    while (e-b > BINARY_SEARCH_THRESH) {
	i = (b+e)>>1;
	if (tg[i].wid < w)
	    b = i+1;
	else if (tg[i].wid > w)
	    e = i;
	else
	    return i;
    }
    
    for (i = b; (i < e) && (tg[i].wid != w); i++);
    return ((i < e) ? i : -1);
}

/* w1, w2, w3 are dictionary wids */
int32 lm3g_tg_score (lm_t *lm, int32 w1, int32 w2, int32 w3)
{
    int32 i, n, score;
    trigram_t *tg;
    tginfo_t *tginfo, *prev_tginfo;
    
    if ((lm->tcount <= 0) || (w1 < 0))
	return (lm3g_bg_score (lm, w2, w3));
    
    /* lm->n_tg_score++; */
    
    if (w1 >= lm->ucount)
	E_FATAL("dictwid[%d] not in LM\n", w1);
    if (w2 >= lm->ucount)
	E_FATAL("dictwid[%d] not in LM\n", w2);
    if (w3 >= lm->ucount)
	E_FATAL("dictwid[%d] not in LM\n", w3);
    
    prev_tginfo = NULL;
    for (tginfo = lm->tginfo[w2]; tginfo; tginfo = tginfo->next) {
	if (tginfo->w1 == w1)
	    break;
	prev_tginfo = tginfo;
    }
    
    if (! tginfo) {
    	load_tginfo (lm, w1, w2);
	tginfo = lm->tginfo[w2];
    } else if (prev_tginfo) {
	prev_tginfo->next = tginfo->next;
	tginfo->next = lm->tginfo[w2];
	lm->tginfo[w2] = tginfo;
    }

    tginfo->used = 1;
    
    /* Trigrams for w1,w2 now pointed to by tginfo */
    n = tginfo->n_tg;
    tg = tginfo->tg;
    if ((i = find_tg (tg, n, w3)) >= 0) {
	score = lm->prob3[tg[i].prob3].l;
    } else {
	/* lm->n_tg_bo++; */
	score = tginfo->bowt + lm3g_bg_score(lm, w2, w3);
    }

    return (score);
}

int lm3g_word_str_size(lm_t *lm)
{
    return lm->n_word_str;
}
