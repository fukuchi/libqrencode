AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS=-I m4

SUBDIRS = .

if BUILD_TESTS
SUBDIRS += tests
endif

lib_LTLIBRARIES = libqrencode.la

libqrencode_la_SOURCES = qrencode.c qrencode_inner.h \
						 qrinput.c qrinput.h \
						 bitstream.c bitstream.h \
						 qrspec.c qrspec.h \
						 rscode.c rscode.h \
						 split.c split.h \
						 mask.c mask.h \
						 mqrspec.c mqrspec.h \
						 mmask.c mmask.h

libqrencode_la_LDFLAGS = -version-number $(MAJOR_VERSION):$(MINOR_VERSION):$(MICRO_VERSION)

include_HEADERS = qrencode.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libqrencode.pc

EXTRA_DIST = libqrencode.pc.in autogen.sh configure.ac acinclude.m4 \
			 Makefile.am tests/Makefile.am qrencode.spec.in qrencode.spec \
			 qrencode.1.in Doxyfile tests/test_all.sh

if BUILD_TOOLS
bin_PROGRAMS = qrencode
qrencode_SOURCES = qrenc.c
qrencode_CFLAGS = $(png_CFLAGS)
qrencode_LDADD = libqrencode.la $(png_LIBS)
man1_MANS = qrencode.1
endif
