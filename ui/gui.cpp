#include <gtk/gtk.h> // -*- tab-width:2 coding: viscii mode: c++ -*-
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "config.h"
#include <spell.h>
#include <vspell.h>

using namespace std;

class MyText : public Text
{
public:
	MyText(VSpell* vs):Text(vs) {}

protected:
	virtual bool ui_syllable_check();
	virtual bool ui_word_check();

	void show_syllables();
	void show_words();
	void show_wrong_words();

	void iter_at(GtkTextIter &iter,int pos);
};

class MyTextFactory : public TextFactory
{
public:
	Text* create(VSpell *vs) const {
		return new MyText(vs);
	}
};

static GtkTextTagTable *tagtable_main;
static GtkTextBuffer *textbuffer_main;
static GtkWidget *textview_main;
static GtkWidget *log_main;
static MyTextFactory myfactory;
static VSpell vspell(myfactory);

void MyText::iter_at(GtkTextIter &iter,int pos)
{
	gtk_text_buffer_get_iter_at_offset(textbuffer_main,&iter,offset+pos);
}

void MyText::show_syllables()
{
	Suggestions &sugg = suggestions;
	int cc,ii,nn,i,n = sugg.size();
	GtkTextIter start,end;
	for (i = 0;i < n;i ++) {
		int id = sugg[i].id;
		int from = st[id].start;
		int len = strlen(sarch[st[id].id]);
		printf("Syllable Mispelled: %d (%s) at %d\n",
					 id,sarch[st[id].id],from);
		iter_at(start,from);
		iter_at(end,from+len);
		gtk_text_buffer_apply_tag_by_name (textbuffer_main, "mispelled",
																			 &start,&end);
	}
}

void MyText::show_words()
{
	int n,cc,i,nn;
	GtkTextIter start,end;

	n = seg.size();
	cc = 0;
	for (i = 0;i < n;i ++) {
		nn = seg[i].node.node->get_syllable_count();
		if (nn == 1) {
			cc += nn;
			continue;
		}
		int from = st[cc].start;
		int to = st[cc+nn-1].start+strlen(sarch[st[cc+nn-1].id]);
		printf("From %d to %d(%d)\n",from,to,n);
		iter_at(start,from);
		iter_at(end,to);
		gtk_text_buffer_apply_tag_by_name (textbuffer_main, "word",
																			 &start,&end);
		cc += nn;
	}
}

void MyText::show_wrong_words()
{
	int n,cc,nn,i,ii;
	GtkTextIter start,end;

	Suggestions &sugg = suggestions;
	n = sugg.size();
	for (i = 0;i < n;i ++) {
		int id = sugg[i].id;
		for (cc = ii = 0;ii < id;ii ++)
			cc += seg[ii].node.node->get_syllable_count();

		nn = seg[id].node.node->get_syllable_count();

		int from = st[cc].start;
		int to = st[cc+nn-1].start+strlen(sarch[st[cc+nn-1].id]);
		printf("Mispelled at %d\n",id);
		iter_at(start,from);
		iter_at(end,to);
		gtk_text_buffer_apply_tag_by_name (textbuffer_main, "mispelled",
																			 &start,&end);
	}
}

static void button_reset_callback (GtkWidget *button, gpointer data)
{
	GtkTextIter start,end;
	gtk_text_buffer_get_start_iter(textbuffer_main,&start);
	gtk_text_buffer_get_end_iter(textbuffer_main,&end);
	gtk_text_buffer_remove_all_tags (textbuffer_main, &start,&end);
}

static void button_spell_callback (GtkWidget *button, gpointer data)
{

	GtkTextIter start,end;
	gtk_text_buffer_get_start_iter(textbuffer_main,&start);
	gtk_text_buffer_get_end_iter(textbuffer_main,&end);

	button_reset_callback(NULL,NULL);

	gchar *buffer = gtk_text_buffer_get_text(textbuffer_main,&start,&end,FALSE);
	//int len = g_utf8_strlen(buffer,-1);

	vspell.check(buffer);
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

	//	sarch["<root>"];

	vspell.init();

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

bool MyText::ui_syllable_check()
{
	show_syllables();
	return true;
}

bool MyText::ui_word_check()
{
	show_words();
	show_wrong_words();
	return true;
}
