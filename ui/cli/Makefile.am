bin_PROGRAMS =  vspell #vspell-gtk

INCLUDES = -I$(top_builddir)/libvspell -I$(top_builddir)/libsrilm #@VSPELL_CFLAGS@ 

#vspell_gtk_SOURCES = gui.cpp
#vspell_gtk_LDADD = $(top_builddir)/libvspell/libvspell.a $(top_builddir)/libsrilm/libsrilm.a  @VSPELL_LIBS@

vspell_SOURCES = main.cpp
vspell_LDADD = $(top_builddir)/libvspell/libvspell.a $(top_builddir)/libsrilm/libsrilm.a
