AUTOMAKE_OPTIONS = foreign

SUBDIRS = . tests

lib_LTLIBRARIES = libqrencode.la

libqrencode_la_SOURCES = qrencode.c qrencode_inner.h \
						 qrinput.c qrinput.h \
						 bitstream.c bitstream.h \
						 qrspec.c qrspec.h \
						 rscode.c rscode.h
libqrencode_la_LDFLAGS = -version-number $(MAJOR_VERSION):$(MINOR_VERSION):$(MICRO_VERSION)

include_HEADERS = qrencode.h
