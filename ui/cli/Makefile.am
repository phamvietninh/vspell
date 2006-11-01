bin_PROGRAMS =  vspell #vspell-gtk

INCLUDES = -I$(top_builddir)/libvspell -I$(top_builddir)/srilm/include #@VSPELL_CFLAGS@ 

#vspell_gtk_SOURCES = gui.cpp
#vspell_gtk_LDADD = $(top_builddir)/libvspell/libvspell.a $(top_builddir)/libsrilm/libsrilm.a  @VSPELL_LIBS@

vspell_SOURCES = main.cpp
SRILM_LIBADD = $(top_builddir)/srilm/lib/i686-gcc4/libdstruct.a $(top_builddir)/srilm/lib/i686-gcc4/libflm.a $(top_builddir)/srilm/lib/i686-gcc4/liblattice.a $(top_builddir)/srilm/lib/i686-gcc4/libmisc.a $(top_builddir)/srilm/lib/i686-gcc4/liboolm.a
vspell_LDADD = $(top_builddir)/libvspell/libvspell.a $(SRILM_LIBADD)
