bin_PROGRAMS = vspell-gtk vspell

INCLUDES = -I$(top_builddir)/libvspell @VSPELL_CFLAGS@ -I$(top_builddir)/libsrilm 

vspell_gtk_SOURCES = gui.cpp
vspell_gtk_LDADD = $(top_builddir)/libvspell/libvspell.a $(top_builddir)/libsrilm/libsrilm.a  @VSPELL_LIBS@

vspell_SOURCES = vspell.cpp cui.cpp
vspell_LDADD = $(top_builddir)/libvspell/libvspell.a $(top_builddir)/libsrilm/libsrilm.a