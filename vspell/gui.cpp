#include <gtk/gtk.h> // -*- tab-width:2 coding: viscii mode: c++ -*-
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "config.h"
#include <spell.h>
#include <sentence.h>
#include <wordnode.h>
#include <wfst.h>

using namespace std;

static GtkTextTagTable *tagtable_main;
static GtkTextBuffer *textbuffer_main;
static GtkWidget *textview_main;
static GtkWidget *log_main;
static char *viscii_str = "áàäãÕâ¤¥¦ç§å¡¢ÆÇ£éèë¨©êª«¬­®íìïî¸óòöõ÷ô¯°±²µ½¾¶·ŞşúùüûøßÑ×ØæñıÏÖÛÜğÁÀÄÃ€Â„…†‡Å‚ƒÉÈËˆ‰ÊŠ‹ŒÍÌ›Î˜ÓÒ™ šÔ‘’“´•–—³”ÚÙœ¿º»¼ÿ¹İŸĞ";
static gunichar unicode_str[] = {
	 225, 224,7843, 227,7841,
	 226,7845,7847,7849,7851,7853,
	 259,7855,7857,7859,7861,7863,
	 233, 232,7867,7869,7865,
	 234,7871,7873,7875,7877,
  7879, 237, 236,7881, 297,7883,
	 243, 242,7887, 245,7885,
	 244,7889,7891,7893,7895,
  7897, 417,7899,7901,7903,7905,
  7907, 250, 249,7911, 361,7909,
	 432,7913,7915,7917,7919,
  7921, 253,7923,7927,7929,7925,
	 273,
	 193, 192,7842, 195,7840,
	 194,7844,7846,7848,7850,7852,
	 258,7854,7856,7858,7860,7862,
	 201, 200,7866,7868,7864,
	 202,7870,7872,7874,7876,7878,
	 205, 204,7880, 296,7882,
	 211, 210,7886, 213,7884,
	 212,7888,7890,7892,7894,
  7896, 416,7898,7900,7902,7904,
  7906, 218, 217,7910, 360,7908,
	 431,7912,7914,7916,7918,7920,
	 221,7922,7926,7928,7924,
	 272,
  0
};

void viet_utf8_to_viscii(const gchar *in,char *out) // pre-allocated
{
  const gchar *p = in;
  gunichar ch;
  int i,n = strlen(viscii_str);
  while ((ch = g_utf8_get_char(p)) != 0) {
    p = g_utf8_next_char(p);
    if (ch < 128) {
      *out++ = ch;
      continue;
    }
    for (i = 0;i < n;i ++)
      if (unicode_str[i] == ch) {
				*out++ = viscii_str[i];
				break;
      }

    if (i >= n) {
      fprintf(stderr,"Warning: unexpected unicode character %d",ch);
      *out++ = (unsigned char)ch;
    }
  }
  *out = 0;
}

void viet_viscii_to_utf8(const char *in,gchar *out) // pre-allocated
{
  unsigned char *p = (unsigned char*)in;
  unsigned char ch;
  int i,n = strlen(viscii_str);
  while ((ch = *p) != 0) {
    p++;
    if (ch < 128) {
      *out++ = ch;
      continue;
    }
    for (i = 0;i < n;i ++)
      if ((unsigned char)viscii_str[i] == ch) {
				g_unichar_to_utf8(unicode_str[i],out);
				out = g_utf8_next_char(out);
				break;
      }

    if (i >= n) {
      fprintf(stderr,"Warning: unexpected viscii character %d",ch);
			g_unichar_to_utf8(ch,out);
			out = g_utf8_next_char(out);
    }
  }
  *out = 0;
}

char* viet_to_viscii(const char *in)
{
	static char buffer[1000];
	if (g_utf8_strlen(in,-1) >= 1000)
		return "";
	viet_utf8_to_viscii(in,buffer);
	return buffer;
}

char* viet_to_utf8(const char *in)
{
	static char buffer[6000];
	if (strlen(in) >= 1000)
		return "";
	viet_viscii_to_utf8(in,buffer);
	return buffer;
}


static void button_reset_callback (GtkWidget *button, gpointer data)
{
  GtkTextIter start,end;
  gtk_text_buffer_get_start_iter(textbuffer_main,&start);
  gtk_text_buffer_get_end_iter(textbuffer_main,&end);
	gtk_text_buffer_remove_all_tags (textbuffer_main, &start,&end);
}

void text_show_syllables(const Sentence &st,const Suggestions &sugg)
{
	int cc,ii,nn,i,n = sugg.size();
	GtkTextIter start,end;
	for (i = 0;i < n;i ++) {
		int id = sugg[i].id;
		int from = st[id].start;
		int len = strlen(sarch[st[id].id]);
		printf("Syllable Mispelled: %d (%s) at %d\n",
					 id,sarch[st[id].id],from);
		gtk_text_buffer_get_iter_at_offset(textbuffer_main,&start,from);
		gtk_text_buffer_get_iter_at_offset(textbuffer_main,&end,from+len);
		gtk_text_buffer_apply_tag_by_name (textbuffer_main, "mispelled",
																			 &start,&end);
	}
}

void text_show_words(const Sentence &st,const Segmentation &seg)
{
	int n,cc,i,nn;
	GtkTextIter start,end;

	n = seg.items.size();
	cc = 0;
	for (i = 0;i < n;i ++) {
		nn = seg.items[i].state->get_syllable_count();
		if (nn == 1) {
			cc += nn;
			continue;
		}
		int from = st[cc].start;
		int to = st[cc+nn-1].start+strlen(sarch[st[cc+nn-1].id]);
		printf("From %d to %d(%d)\n",from,to,n);
		gtk_text_buffer_get_iter_at_offset(textbuffer_main,&start,from);
		gtk_text_buffer_get_iter_at_offset(textbuffer_main,&end,to);
		gtk_text_buffer_apply_tag_by_name (textbuffer_main, "word",
																			 &start,&end);
		cc += nn;
	}
}

void text_show_wrong_words(const Sentence &st,const Segmentation &seg,const Suggestions &sugg)
{
	int n,cc,nn,i,ii;
	GtkTextIter start,end;

	n = sugg.size();
	for (i = 0;i < n;i ++) {
		int id = sugg[i].id;
		for (cc = ii = 0;ii < id;ii ++)
			cc += seg.items[ii].state->get_syllable_count();

		nn = seg.items[id].state->get_syllable_count();

		int from = st[cc].start;
		int to = st[cc+nn-1].start+strlen(sarch[st[cc+nn-1].id]);
		printf("Mispelled at %d\n",id);
		gtk_text_buffer_get_iter_at_offset(textbuffer_main,&start,from);
		gtk_text_buffer_get_iter_at_offset(textbuffer_main,&end,to);
		gtk_text_buffer_apply_tag_by_name (textbuffer_main, "mispelled",
																			 &start,&end);
	}
}

void sentence_process(const char *pp)
{
	// preprocess
	Sentence st(pp);
	Segmentation seg;
	Words words;
	Suggestions sugg;
	st.standardize();
	st.tokenize();

	// syllable checking
	spell_check1(st,sugg);

	// show mispelled syllables
	text_show_syllables(st,sugg);

	// try segmentation
	if (/*n == 0*/1) {
		WFST wfst;
		wfst.enable_ngram();
		wfst.set_wordlist(get_root());
		wfst.get_all_words(st,words);
		words.print();
		wfst.segment_best(st,words,seg);
			
		seg.print(cerr,st);

		// show segmentation
		text_show_words(st,seg);

		// word checking
		sugg.clear();
		spell_check2(st,seg,sugg);
		
		// show mispelled words
		text_show_wrong_words(st,seg,sugg);
	}
}

static void button_spell_callback (GtkWidget *button, gpointer data)
{
  GtkTextIter start,end;
  gtk_text_buffer_get_start_iter(textbuffer_main,&start);
  gtk_text_buffer_get_end_iter(textbuffer_main,&end);

	button_reset_callback(NULL,NULL);

  gchar *buffer = gtk_text_buffer_get_text(textbuffer_main,&start,&end,FALSE);
  int len = g_utf8_strlen(buffer,-1);
  char *pp = viet_to_viscii(buffer);

	vector<string> pps;
	sentences_split(pp,pps);
	int pps_i,pps_len = pps.size();
	for (pps_i = 0;pps_i < pps_len;pps_i ++)
		sentence_process(pps[pps_i].c_str());
}

static void button_exit_callback (GtkWidget *button, gpointer data)
{
  exit(0);
}

static void window_destroy_callback (GtkObject *object,
																		 gpointer user_data)
{
  exit(0);
}

static void button_search_callback(GtkWidget *button, gpointer data);

int main(int argc,char **argv)
{
  gtk_init(&argc,&argv);

  dic_init(new FuzzyWordNode(sarch["<root>"]));

  cerr << "Loading dictionary... ";
  get_root()->load("wordlist.wl");
  cerr << "done" << endl;
  cerr << "Loading ngram... ";
  File f("ngram","rt");
  ngram.write(f);
  cerr << "done" << endl;
	sarch.set_blocked(true);

  GtkWidget *window_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window_main),400,200);
  g_signal_connect(window_main,"destroy",G_CALLBACK(window_destroy_callback),NULL);

  GtkWidget *vbox_main = gtk_vbox_new(FALSE,10);
  gtk_container_add(GTK_CONTAINER(window_main),vbox_main);


	GtkWidget *hbox_search = gtk_hbox_new(FALSE,10);
	gtk_box_pack_start(GTK_BOX(vbox_main),hbox_search,FALSE,TRUE,10);

	GtkWidget *entry_search = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox_search),entry_search,TRUE,TRUE,10);
	GtkWidget *button_search = gtk_button_new_with_mnemonic("_Search");
	g_signal_connect(button_search,"clicked",
									 G_CALLBACK(button_search_callback),entry_search);
	gtk_box_pack_start(GTK_BOX(hbox_search),button_search,FALSE,TRUE,10);

	log_main = gtk_label_new("");
	gtk_label_set_line_wrap(GTK_LABEL(log_main),true);
	gtk_box_pack_start(GTK_BOX(vbox_main),log_main,FALSE,FALSE,10);
	
  
	GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL,NULL);
  gtk_box_pack_start(GTK_BOX(vbox_main),scrolled_window,TRUE,TRUE,0);

  tagtable_main = gtk_text_tag_table_new();
  textbuffer_main = gtk_text_buffer_new(tagtable_main);
	gtk_text_buffer_create_tag (textbuffer_main, "mispelled",
															"weight", PANGO_WEIGHT_BOLD,
															"style", PANGO_STYLE_ITALIC,
															"foreground", "red",
															NULL);
	gtk_text_buffer_create_tag (textbuffer_main, "word",
															"underline", (gboolean)TRUE,
															NULL);

  textview_main = gtk_text_view_new_with_buffer(textbuffer_main);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview_main),GTK_WRAP_WORD);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),textview_main);

  GtkWidget *hbox_command = gtk_hbox_new(TRUE,10);
  gtk_box_pack_start(GTK_BOX(vbox_main),hbox_command,FALSE,TRUE,0);

  GtkWidget *button_spell = gtk_button_new_with_mnemonic("_Check");
  g_signal_connect(button_spell,"clicked",G_CALLBACK(button_spell_callback),NULL);
  GtkWidget *button_reset = gtk_button_new_with_mnemonic("_Reset");
  g_signal_connect(button_reset,"clicked",G_CALLBACK(button_reset_callback),NULL);
  GtkWidget *button_exit = gtk_button_new_with_mnemonic("_Exit");
  g_signal_connect(button_exit,"clicked",G_CALLBACK(button_exit_callback),NULL);
  gtk_box_pack_start(GTK_BOX(hbox_command),button_spell,FALSE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(hbox_command),button_reset,FALSE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(hbox_command),button_exit,FALSE,TRUE,0);

  gtk_widget_show_all(window_main);
  gtk_main();
}

void button_search_callback(GtkWidget *button, gpointer data)
{
	GtkWidget *entry_search = GTK_WIDGET(data);
	const char *text = gtk_entry_get_text(GTK_ENTRY(data));

	string is(viet_to_viscii(text));
	int p = 0;
	string s;
	vector<strid> ids;
	while (p < is.size()) {
		int pp = is.find(' ',p);
		if (pp == string::npos)
			pp = is.size();
		s = is.substr(p,pp-p);
		p = pp+1;
		strid id = sarch[s];
		if (!sarch.in_dict(id)) {
			char *str = g_strdup_printf("%s not found",viet_to_utf8(s.c_str()));
			gtk_label_set_text(GTK_LABEL(log_main),str);
			g_free(str);
			return;
		}
		ids.push_back(id);
	}

	WordNodePtr ptr;
	ptr = get_root()->follow_syllables(ids);
	if (ptr) {
		char *str = g_strdup_printf("Word %s found with prob %.02f.",text,ptr->get_prob());
		gtk_label_set_text(GTK_LABEL(log_main),str);
		g_free(str);
	} else
		gtk_label_set_text(GTK_LABEL(log_main),"Word not found.");
}
