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

	void show_wrong_syllables(unsigned p);
	void show_words();
	void show_wrong_words(unsigned p);

	void iter_at(GtkTextIter &iter,int pos);
	string word_to_utf8(unsigned seg_id);
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
static GtkWidget *spell_entry;
static GtkWidget *ignore_button,*ignore_all_button,*spell_button,*open_button;
static GtkListStore *list_store;
static MyTextFactory myfactory;
static VSpell vspell(myfactory);

static void text_reset(GtkTextBuffer *textbuffer_main)
{
	GtkTextIter start,end;
	gtk_text_buffer_get_start_iter(textbuffer_main,&start);
	gtk_text_buffer_get_end_iter(textbuffer_main,&end);
	gtk_text_buffer_remove_all_tags (textbuffer_main, &start,&end);
}

void MyText::iter_at(GtkTextIter &iter,int pos)
{
	gtk_text_buffer_get_iter_at_offset(textbuffer_main,&iter,offset+pos);
}

void MyText::show_wrong_syllables(unsigned p)
{
	Suggestions &sugg = suggestions;
	int cc,ii,nn,i,n = sugg.size();
	gtk_text_buffer_set_text (textbuffer_main,
														vspell->get_utf8_text().c_str(),
														strlen(vspell->get_utf8_text().c_str()));
	GtkTextIter start,end;
	iter_at(start,0);
	iter_at(end,length);
	gtk_text_buffer_apply_tag_by_name(textbuffer_main,
																		"bg-syllable", &start, &end);
	for (i = 0;i < n;i ++) {
		int id = sugg[i].id;
		int from = st[id].start;
		int len = strlen(get_sarch()[st[id].id]);
		printf("Syllable Mispelled: %d (%s) at %d\n",
					 id,get_sarch()[st[id].id],from);
		iter_at(start,from);
		iter_at(end,from+len);
		gtk_text_buffer_apply_tag_by_name (textbuffer_main,
																			 (i == p ? "mispelled" : "mispelled2"),
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
		int to = st[cc+nn-1].start+strlen(get_sarch()[st[cc+nn-1].id]);
		printf("From %d to %d(%d)\n",from,to,n);
		iter_at(start,from);
		iter_at(end,to);
		gtk_text_buffer_apply_tag_by_name (textbuffer_main, "word",
																			 &start,&end);
		cc += nn;
	}
}

void MyText::show_wrong_words(unsigned p)
{
	int n,cc,nn,i,ii;
	GtkTextIter start,end;

	Suggestions &sugg = suggestions;
	gtk_text_buffer_set_text (textbuffer_main, 
														vspell->get_utf8_text().c_str(),
														strlen(vspell->get_utf8_text().c_str()));
	iter_at(start,0);
	iter_at(end,length);
	gtk_text_buffer_apply_tag_by_name(textbuffer_main,
																		"bg-word", &start, &end);
																		
	show_words();
	n = sugg.size();
	for (i = 0;i < n;i ++) {
		int id = sugg[i].id;
		for (cc = ii = 0;ii < id;ii ++)
			cc += seg[ii].node.node->get_syllable_count();

		nn = seg[id].node.node->get_syllable_count();

		int from = st[cc].start;
		int to = st[cc+nn-1].start+strlen(get_sarch()[st[cc+nn-1].id]);
		printf("Mispelled at %d\n",id);
		iter_at(start,from);
		iter_at(end,to);
		gtk_text_buffer_apply_tag_by_name (textbuffer_main, 
																			 (i == p ? "mispelled" : "mispelled2"),
																			 &start,&end);
	}
}

static void button_reset_callback (GtkWidget *button, gpointer data)
{
	text_reset(textbuffer_main);
}

static bool is_checking = true;
static bool ignore = false;
static bool ignore_all = false;
static bool processed = false;

static void set_state(bool spellcheck)
{
	GtkTextIter start,end;

	if (spellcheck == is_checking)
		return;

	is_checking = spellcheck;
	gtk_text_buffer_get_start_iter(textbuffer_main,&start);
	gtk_text_buffer_get_end_iter(textbuffer_main,&end);
	gtk_text_buffer_remove_all_tags (textbuffer_main, &start,&end);
	
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview_main),!is_checking);
	gtk_widget_set_sensitive(GTK_WIDGET(open_button),!is_checking);
	gtk_widget_set_sensitive(GTK_WIDGET(spell_button),!is_checking);
	gtk_widget_set_sensitive(GTK_WIDGET(ignore_button),is_checking);
	gtk_widget_set_sensitive(GTK_WIDGET(ignore_all_button),is_checking);
	ignore = true;
}

static void button_spell_callback (GtkWidget *button, gpointer data)
{
	GtkTextIter start,end;

	set_state(true);

	gtk_text_buffer_get_start_iter(textbuffer_main,&start);
	gtk_text_buffer_get_end_iter(textbuffer_main,&end);

	gchar *buffer = gtk_text_buffer_get_text(textbuffer_main,&start,&end,FALSE);
	//int len = g_utf8_strlen(buffer,-1);

	vspell.check(buffer);
	set_state(false);
	gtk_text_buffer_set_text (textbuffer_main, vspell.get_utf8_text().c_str(),strlen(vspell.get_utf8_text().c_str()));

}

static void button_ignore_all_callback(GtkWidget *button,gpointer data)
{
	//set_state(false);
	ignore_all = true;
}

static void button_spell_accept_callback (GtkWidget *button, gpointer data)
{
		processed = true;
}

static void spell_entry_activate_callback(GtkEntry *entry, gpointer data)
{
	button_spell_accept_callback(NULL,NULL);
}

static void button_ignore_callback (GtkWidget *button, gpointer data)
{
	ignore = true;
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

static void
candidates_row_activated (GtkTreeView *treeview,
													GtkTreePath *path,
													GtkTreeViewColumn *arg2,
													gpointer user_data)
{
	GtkTreeIter iter;
	const char *s;
	if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(list_store),&iter,path))
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(list_store),&iter,0,&s,-1);
	gtk_entry_set_text(GTK_ENTRY(spell_entry),s);
}


int main(int argc,char **argv)
{
	gtk_init(&argc,&argv);

	//	sarch["<root>"];

	vspell.init();

	GtkWidget *window_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window_main),10);
	gtk_window_set_default_size(GTK_WINDOW(window_main),400,200);
	g_signal_connect(window_main,"destroy",G_CALLBACK(window_destroy_callback),NULL);

	GtkWidget *vbox_main = gtk_vbox_new(FALSE,10);
	gtk_container_add(GTK_CONTAINER(window_main),vbox_main);

	// Search box
	GtkWidget *hbox_search = gtk_hbox_new(FALSE,10);
	gtk_box_pack_start(GTK_BOX(vbox_main),hbox_search,FALSE,TRUE,0);

	GtkWidget *entry_search = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox_search),entry_search,TRUE,TRUE,0);
	GtkWidget *button_search = gtk_button_new_with_mnemonic("_Search");
	g_signal_connect(button_search,"clicked",
									 G_CALLBACK(button_search_callback),entry_search);
	gtk_box_pack_start(GTK_BOX(hbox_search),button_search,FALSE,TRUE,0);

	log_main = gtk_label_new("");
	gtk_label_set_line_wrap(GTK_LABEL(log_main),true);
	gtk_box_pack_start(GTK_BOX(vbox_main),log_main,FALSE,FALSE,0);
	
	// Text box+Spell box
	GtkWidget *paned = gtk_hpaned_new();
	gtk_paned_set_position(GTK_PANED(paned),300);
	gtk_box_pack_start(GTK_BOX(vbox_main),paned,TRUE,TRUE,0);
	
	// --textbox
	GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL,NULL);
	gtk_paned_add1(GTK_PANED(paned),scrolled_window);

	tagtable_main = gtk_text_tag_table_new();
	textbuffer_main = gtk_text_buffer_new(tagtable_main);
	gtk_text_buffer_create_tag (textbuffer_main, "mispelled",
															"weight", PANGO_WEIGHT_BOLD,
															"style", PANGO_STYLE_ITALIC,
															"foreground", "red",
															NULL);
	gtk_text_buffer_create_tag (textbuffer_main, "mispelled2",
															"weight", PANGO_WEIGHT_BOLD,
															"style", PANGO_STYLE_ITALIC,
															NULL);
	gtk_text_buffer_create_tag (textbuffer_main, "word",
															"underline", (gboolean)TRUE,
															NULL);
	gtk_text_buffer_create_tag (textbuffer_main, "bg-word",
															"background", "grey",
															NULL);
	gtk_text_buffer_create_tag (textbuffer_main, "bg-syllable",
															"background", "lightgrey",
															NULL);

	textview_main = gtk_text_view_new_with_buffer(textbuffer_main);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview_main),GTK_WRAP_WORD);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),textview_main);

	// --spellbox
	GtkWidget *spell_vbox = gtk_vbox_new(FALSE,10);
	gtk_paned_add2(GTK_PANED(paned),spell_vbox);

	GtkWidget *spell_entry_hbox = gtk_hbox_new(FALSE,5);
	gtk_box_pack_start(GTK_BOX(spell_vbox),spell_entry_hbox,FALSE,FALSE,0);

	spell_entry = gtk_entry_new();
	g_signal_connect(G_OBJECT(spell_entry),"activate",G_CALLBACK(spell_entry_activate_callback),NULL);
	gtk_box_pack_start(GTK_BOX(spell_entry_hbox),spell_entry,TRUE,TRUE,0);

	GtkWidget *spell_entry_button = gtk_button_new();
	GtkWidget *spell_entry_button_image = gtk_image_new_from_stock(GTK_STOCK_OK,GTK_ICON_SIZE_BUTTON);
	g_signal_connect(spell_entry_button,"clicked",G_CALLBACK(button_spell_accept_callback),NULL);
	gtk_container_add(GTK_CONTAINER(spell_entry_button),spell_entry_button_image);
	gtk_box_pack_start(GTK_BOX(spell_entry_hbox),spell_entry_button,FALSE,FALSE,0);

	GtkWidget *view = gtk_scrolled_window_new(NULL,NULL);
	gtk_box_pack_start(GTK_BOX(spell_vbox),view,TRUE,TRUE,0);

	GtkWidget *w = gtk_tree_view_new();
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(view),w);
	list_store = gtk_list_store_new(1,G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(w),GTK_TREE_MODEL(list_store));
	gtk_tree_view_append_column(GTK_TREE_VIEW(w),
															gtk_tree_view_column_new_with_attributes("Candidates",
																																			 gtk_cell_renderer_text_new(),
																																			 "text",
																																			 0,
																																			 NULL));
	g_signal_connect(G_OBJECT(w),"row-activated",
									 G_CALLBACK(candidates_row_activated),
									 NULL);

	// Commands
	GtkWidget *hbox_command = gtk_hbox_new(TRUE,10);
	gtk_box_pack_start(GTK_BOX(vbox_main),hbox_command,FALSE,TRUE,0);

	open_button = gtk_button_new_with_mnemonic("_Open");
	//g_signal_connect(button_spell,"clicked",G_CALLBACK(button_spell_callback),NULL);
	gtk_box_pack_start(GTK_BOX(hbox_command),open_button,FALSE,TRUE,0);
	spell_button = gtk_button_new_with_mnemonic("_Check");
	g_signal_connect(spell_button,"clicked",G_CALLBACK(button_spell_callback),NULL);
	gtk_box_pack_start(GTK_BOX(hbox_command),spell_button,FALSE,TRUE,0);
	//GtkWidget *button_reset = gtk_button_new_with_mnemonic("_Reset");
	//g_signal_connect(button_reset,"clicked",G_CALLBACK(button_reset_callback),NULL);
	//gtk_box_pack_start(GTK_BOX(hbox_command),button_reset,FALSE,TRUE,0);
	ignore_button = gtk_button_new_with_mnemonic("_Ignore");
	g_signal_connect(ignore_button,"clicked",G_CALLBACK(button_ignore_callback),NULL);
	gtk_box_pack_start(GTK_BOX(hbox_command),ignore_button,FALSE,TRUE,0);
	ignore_all_button = gtk_button_new_with_mnemonic("Ignore _All");
	g_signal_connect(ignore_all_button,"clicked",G_CALLBACK(button_ignore_all_callback),NULL);
	gtk_box_pack_start(GTK_BOX(hbox_command),ignore_all_button,FALSE,TRUE,0);
	GtkWidget *button_exit = gtk_button_new_with_mnemonic("_Exit");
	g_signal_connect(button_exit,"clicked",G_CALLBACK(button_exit_callback),NULL);
	gtk_box_pack_start(GTK_BOX(hbox_command),button_exit,FALSE,TRUE,0);

	set_state(false);

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
		strid id = get_sarch()[s];
		if (!get_sarch().in_dict(id)) {
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

void candidates_reset()
{
	gtk_list_store_clear(list_store);
}

void candidates_add(const gchar *s)
{
	GtkTreeIter iter;
	gtk_list_store_append(list_store,&iter);
	gtk_list_store_set(list_store,&iter,
										 0,s,
										 -1);
}

bool MyText::ui_syllable_check()
{
	unsigned i,n = suggestions.size();
	for (i = 0;i < n;i ++) {
		show_wrong_syllables(i);
		// query
		int from,len;
		from = st[suggestions[i].id].start;
		len = strlen(get_sarch()[st[suggestions[i].id].id]);
		string s = substr(from,len);
		gtk_entry_set_text(GTK_ENTRY(spell_entry),s.c_str());
		set<string> candidates;
		candidates_reset();
		get_syllable_candidates(get_sarch()[st[suggestions[i].id].id],candidates);
		set<string>::iterator iter;
		for (iter = candidates.begin();iter != candidates.end();++ iter)
			candidates_add(viet_to_utf8(iter->c_str()));
		processed = ignore_all = ignore = false;
		while (!gtk_main_iteration() && !ignore && !ignore_all && !processed);

		if (ignore)
			continue;

		if (ignore_all)
			return true;							// force to exit

		if (processed) {
			cerr << "Input: The right one is:" << endl;

			const gchar *s = gtk_entry_get_text(GTK_ENTRY(spell_entry));

			if (*s == 0)
				continue;										// i don't accept an empty string

			replace(st[suggestions[i].id].start, // from
							strlen(get_sarch()[st[suggestions[i].id].get_id()]), // size
							s);					// text
			vspell->add(get_sarch()[viet_to_viscii_force(s)]);

			return false;
		}
		return true;								// some things went wrong
	}
	return !is_checking;
}

string MyText::word_to_utf8(unsigned seg_id)
{
	vector<strid> sylls;
	string s;
	seg[seg_id].node->get_syllables(sylls);
	int i,n = sylls.size();
	for (i = 0;i < n;i ++) {
		if (i)
			s += " ";
		Syllable syll;
		syll.parse(get_sarch()[sylls[i]]);
		s += viet_to_utf8(syll.to_str().c_str());
	}
	return s;
}

bool MyText::ui_word_check()
{
	unsigned i,n = suggestions.size();
	int pos,pos2,count;

	for (i = 0;i < n;i ++) {
		show_wrong_words(i);
		// query
		count = seg[suggestions[i].id].node->get_syllable_count();
		pos = (*seg.we)[seg[suggestions[i].id].id].pos;
		pos2 = pos+count-1;
		int from,len;
		from = st[pos].start;
		len = st[pos2].start+strlen(get_sarch()[st[pos2].id])-from;
		string s = substr(from,len);
		gtk_entry_set_text(GTK_ENTRY(spell_entry),s.c_str());
		candidates_reset();
		candidates_add(word_to_utf8(suggestions[i].id).c_str());
		processed = ignore_all = ignore = false;
		while (!gtk_main_iteration() && !ignore && !ignore_all && !processed);

		if (ignore)
			continue;

		if (ignore_all)
			return true;							// force to exit

		if (processed) {
			string s = gtk_entry_get_text(GTK_ENTRY(spell_entry));

			if (s.empty())
				continue;

			count = seg[suggestions[i].id].node->get_syllable_count();
			pos = (*seg.we)[seg[suggestions[i].id].id].pos;
			pos2 = pos+count-1;

			string::size_type p;
			vector<unsigned> separators;
			while ((p = s.find('|')) != string::npos) {
				separators.push_back(st[pos].start+g_utf8_strlen(s.substr(0,p).c_str(),-1)+offset);
				s.erase(p,1);
			}

			replace(st[pos].start, // from
							st[pos2].start+strlen(get_sarch()[st[pos2].get_id()])-st[pos].start, // size
							s.c_str());					// text
			
			// add separators after replacing the text, to have old separators removed
			vspell->add_word(viet_to_viscii_force(s.c_str()));
			vspell->add_separators(separators);
			return false;								// continue checking
		}
		return true;								// some things went wrong
	}
	return !is_checking;
}
