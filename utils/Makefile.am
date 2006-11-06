LDADD = $(top_builddir)/libvspell/libvspell.a
INCLUDES = -I$(top_builddir)/libvspell @VSPELL_CFLAGS@

bin_PROGRAMS = syllable-test \
	distance-test sentence-test \
	posgen-test\
	tokenize-test \
	std-syllable sc-train keyb-test\
	vspell-check\
	utf8toviscii lm-test lattice-test sc2wngram dag-test
EXTRAS = data.test

std_syllable_SOURCES = std-syllable.cpp
std_syllable_LDADD = $(LDADD)
distance_test_SOURCES = distance-test.cpp
distance_test_LDADD = $(LDADD)
syllable_test_SOURCES = syllable-test.cpp
syllable_test_LDADD = $(LDADD)
sc_train_SOURCES = sc-train.cpp softcount.cpp softcount.h
sc_train_LDADD = $(LDADD)
sentence_test_SOURCES = sentence-test.cpp
sentence_test_LDADD = $(LDADD)
posgen_test_SOURCES = posgen-test.cpp
posgen_test_LDADD = $(LDADD)
tokenize_test_SOURCES = tokenize-test.cpp
tokenize_test_LDADD = $(LDADD)
keyb_test_SOURCES = keyb-test.cpp
keyb_test_LDADD = $(LDADD)
vspell_check_SOURCES = vspell-check.cpp
vspell_check_LDADD = $(LDADD)
utf8toviscii_SOURCES = utf8toviscii.cpp
utf8toviscii_LDADD = $(LDADD)
lm_test_SOURCES = lm-test.cpp
lm_test_LDADD = $(LDADD)
lattice_test_SOURCES = lattice-test.cpp
lattice_test_LDADD = $(LDADD)
sc2wngram_SOURCES = sc2wngram.c
dag_test_SOURCES = dag-test.cpp
dag_test_LDADD = $(LDADD)
