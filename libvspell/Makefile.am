noinst_LIBRARIES = libvspell.a
libvspell_a_SOURCES = types.h\
	syllable.cpp syllable.h\
	dictionary.cpp dictionary.h wordnode.h\
	propername.cpp propername.h\
	sentence.cpp sentence.h\
	distance.cpp distance.h\
	spell.cpp spell.h vspell.h\
	pfs.cpp pfs.h\
	words.cpp\
	warch.cpp\
	softcount.cpp softcount.h\
	tokenize.ll tokenize.h\
	debug.h mystring.h\
	posgen.cpp posgen.h\
	cgen.h cgen.cpp\
	boost\
	keyboard.h keyboard.cpp\
	shuffle.h shuffle.cpp\
	dag.h dag.cpp\
	bellman.cpp bellman.h\
	penalty.h penalty.cpp
INCLUDES = -I$(top_builddir)/libsrilm
libvspell_a_LIBADD = $(top_builddir)/libsrilm/libsrilm.a
#data_DATA = data/wordlist.wl data/ngram data/last-names.lst
