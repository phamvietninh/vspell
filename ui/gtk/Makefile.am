bin_PROGRAMS =  vspell-gtk

PKG_CONFIG=pkg-config

INCLUDES = -I$(top_builddir)/libvspell -I$(top_builddir)/srilm/include @VSPELL_CFLAGS@  `$(PKG_CONFIG) --cflags gtk+-2.0`

vspell_gtk_SOURCES = main.cpp
SRILM_LIBADD = $(top_builddir)/srilm/lib/i686-gcc4/libdstruct.a $(top_builddir)/srilm/lib/i686-gcc4/libflm.a $(top_builddir)/srilm/lib/i686-gcc4/liblattice.a $(top_builddir)/srilm/lib/i686-gcc4/libmisc.a $(top_builddir)/srilm/lib/i686-gcc4/liboolm.a
vspell_gtk_LDADD = $(top_builddir)/libvspell/libvspell.a $(SRILM_LIBADD) @VSPELL_LIBS@
vspell_gtk_LDFLAGS = `$(PKG_CONFIG) --libs gtk+-2.0`

