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
distance_test_SOURCES = distance-test.cpp
syllable_test_SOURCES = syllable-test.cpp
sc_train_SOURCES = sc-train.cpp softcount.cpp softcount.h
sentence_test_SOURCES = sentence-test.cpp
posgen_test_SOURCES = posgen-test.cpp
tokenize_test_SOURCES = tokenize-test.cpp
keyb_test_SOURCES = keyb-test.cpp
vspell_check_SOURCES = vspell-check.cpp
utf8toviscii_SOURCES = utf8toviscii.cpp
lm_test_SOURCES = lm-test.cpp
lattice_test_SOURCES = lattice-test.cpp
sc2wngram_SOURCES = sc2wngram.c
dag_test_SOURCES = dag-test.cpp
