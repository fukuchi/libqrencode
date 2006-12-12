if HAVE_SDL
sdlPROGRAMS = view_qrcode
endif

noinst_PROGRAMS = test_qrinput test_bitstream test_estimatebit \
				  test_qrspec test_rs test_qrencode prof_qrencode \
				  $(sdlPROGRAMS)

test_qrinput_SOURCES = test_qrinput.c common.h
test_qrinput_LDADD = ../libqrencode.la

test_bitstream_SOURCES = test_bitstream.c common.h
test_bitstream_LDADD = ../bitstream.o

test_estimatebit_SOURCES = test_estimatebit.c common.h
test_estimatebit_LDADD = ../libqrencode.la

test_qrspec_SOURCES = test_qrspec.c common.h
test_qrspec_LDADD = ../qrspec.o

test_rs_SOURCES = test_rs.c common.h
test_rs_LDADD = ../libqrencode.la

test_qrencode_SOURCES = test_qrencode.c common.h
test_qrencode_LDADD = ../libqrencode.la

prof_qrencode_SOURCES = prof_qrencode.c
prof_qrencode_LDADD = ../libqrencode.la

if HAVE_SDL
view_qrcode_SOURCES = view_qrcode.c common.h
view_qrcode_CFLAGS= $(SDL_CFLAGS)
view_qrcode_LDADD = ../libqrencode.la $(SDL_LIBS)
endif
