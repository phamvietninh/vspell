bin_PROGRAMS = vspell
vspell_SOURCES = gui.cpp
vspell_LDADD = $(top_builddir)/libvspell/libvspell.a $(top_builddir)/libsrilm/libsrilm.a  @VSPELL_LIBS@
INCLUDES = -I$(top_builddir)/libvspell @VSPELL_CFLAGS@ -I$(top_builddir)/libsrilm 
