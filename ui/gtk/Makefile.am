bin_PROGRAMS =  vspell-gtk

INCLUDES = -I$(top_builddir)/libvspell -I$(top_builddir)/libsrilm @VSPELL_CFLAGS@  `pkg-config --cflags gtk+-2.0`

vspell_gtk_SOURCES = main.cpp
vspell_gtk_LDADD = $(top_builddir)/libvspell/libvspell.a $(top_builddir)/libsrilm/libsrilm.a  @VSPELL_LIBS@
vspell_gtk_LDFLAGS = `pkg-config --libs gtk+-2.0`

