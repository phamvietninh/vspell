LDADD = $(top_builddir)/libvspell/libvspell.a $(top_builddir)/libsrilm/libsrilm.a
INCLUDES = -I$(top_builddir)/libsrilm  -I$(top_builddir)/libvspell @VSPELL_CFLAGS@

bin_PROGRAMS = dictionary-test syllable-test wfst-train\
	distance-test sentence-test wfst-test

dictionary_test_SOURCES = dictionary-test.cpp
dictionary_test_LDADD = $(LDADD)
distance_test_SOURCES = distance-test.cpp
distance_test_LDADD = $(LDADD)
syllable_test_SOURCES = syllable-test.cpp
syllable_test_LDADD = $(LDADD)
wfst_train_SOURCES = wfst-train.cpp
wfst_train_LDADD = $(LDADD)
wfst_test_SOURCES = wfst-test.cpp
wfst_test_LDADD = $(LDADD)
sentence_test_SOURCES = sentence-test.cpp
sentence_test_LDADD = $(LDADD)
