AUTOMAKE_OPTIONS = foreign

SUBDIRS = . tests

lib_LTLIBRARIES = libqrencode.la

libqrencode_la_SOURCES = qrencode.c bitstream.c
libqrencode_la_headers = qrencode.h bitstream.h
libqrencode_la_LDFLAGS = -version-info $(MAJOR_VERSION):$(MINOR_VERSION):$(MICRO_VERSION)

pkginclude_HEADERS = qrencode.h
