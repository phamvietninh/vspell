lib_LIBRARIES = libvspell.a
libvspell_a_SOURCES = syllable.cpp dictionary.cpp propername.cpp \
	sentence.cpp distance.cpp spell.cpp wfst.cpp\
	dictionary.h param.h sentence.h wfst.h\
	debug.h distance.h propername.h spell.h
INCLUDES = -I$(top_builddir)/libsrilm
libvspell_a_LIBADD = $(top_builddir)/libsrilm/libsrilm.a
data_DATA = data/wordlist.wl data/ngram data/last-names.lst
