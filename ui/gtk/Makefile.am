bin_PROGRAMS =  vspell-gtk

PKG_CONFIG=pkg-config

INCLUDES = -I$(top_builddir)/libvspell -I$(top_builddir)/srilm/include @VSPELL_CFLAGS@  `$(PKG_CONFIG) --cflags gtk+-2.0`

vspell_gtk_SOURCES = main.cpp
vspell_gtk_LDADD = $(top_builddir)/libvspell/libvspell.la @VSPELL_LIBS@
vspell_gtk_LDFLAGS = `$(PKG_CONFIG) --libs gtk+-2.0`

