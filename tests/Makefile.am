LDADD = $(top_builddir)/libvspell/libvspell.a $(top_builddir)/libsrilm/libsrilm.a
INCLUDES = -I$(top_builddir)/libsrilm  -I$(top_builddir)/libvspell @VSPELL_CFLAGS@

bin_PROGRAMS = dictionary-test syllable-test wfst-train sytrain\
	distance-test sentence-test wfst-test\
	section-test all-words-test posgen-test\
	segm-test tokenize-test shortestpath\
	std-syllable sc-train keyb-test\
	vspell-check
EXTRAS = data.test

std_syllable_SOURCES = std-syllable.cpp
std_syllable_LDADD = $(LDADD)
shortestpath_SOURCES = shortestpath.cpp
shortestpath_LDADD = $(LDADD)
all_words_test_SOURCES = all-words-test.cpp
all_words_test_LDADD = $(LDADD)
section_test_SOURCES = section-test.cpp
section_test_LDADD = $(LDADD)
dictionary_test_SOURCES = dictionary-test.cpp
dictionary_test_LDADD = $(LDADD)
distance_test_SOURCES = distance-test.cpp
distance_test_LDADD = $(LDADD)
syllable_test_SOURCES = syllable-test.cpp
syllable_test_LDADD = $(LDADD)
wfst_train_SOURCES = wfst-train.cpp
wfst_train_LDADD = $(LDADD)
sytrain_SOURCES = sytrain.cpp
sytrain_LDADD = $(LDADD)
sc_train_SOURCES = sc-train.cpp
sc_train_LDADD = $(LDADD)
wfst_test_SOURCES = wfst-test.cpp
wfst_test_LDADD = $(LDADD)
sentence_test_SOURCES = sentence-test.cpp
sentence_test_LDADD = $(LDADD)
posgen_test_SOURCES = posgen-test.cpp
posgen_test_LDADD = $(LDADD)
segm_test_SOURCES = segm-test.cpp
segm_test_LDADD = $(LDADD)
tokenize_test_SOURCES = tokenize-test.cpp
tokenize_test_LDADD = $(LDADD)
keyb_test_SOURCES = keyb-test.cpp
keyb_test_LDADD = $(LDADD)
vspell_check_SOURCES = vspell-check.cpp
vspell_check_LDADD = $(LDADD)
