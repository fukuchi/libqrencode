AUTOMAKE_OPTIONS = foreign

SUBDIRS = . tests

lib_LTLIBRARIES = libqrencode.la

libqrencode_la_SOURCES = qrencode.c datastream.c bitstream.c qrspec.c rscode.c
libqrencode_la_headers = qrencode.h qrencode_inner.h datastream.h bitstream.h \
						 qrspec.h rscode.h
libqrencode_la_LDFLAGS = -version-number $(MAJOR_VERSION):$(MINOR_VERSION):$(MICRO_VERSION)

pkginclude_HEADERS = qrencode.h
