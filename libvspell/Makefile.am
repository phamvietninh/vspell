lib_LIBRARIES = libvspell.a
libvspell_a_SOURCES = \
	syllable.cpp syllable.h\
	dictionary.cpp dictionary.h wordnode.h\
	propername.cpp propername.h\
	sentence.cpp sentence.h\
	distance.cpp distance.h\
	spell.cpp spell.h\
	wfst.cpp wfst.h\
	pfs.cpp pfs.h\
	words.cpp sections.cpp\
	softcount.cpp softcount.h\
	tokenize.ll\
	debug.h
INCLUDES = -I$(top_builddir)/libsrilm
libvspell_a_LIBADD = $(top_builddir)/libsrilm/libsrilm.a
data_DATA = data/wordlist.wl data/ngram data/last-names.lst
