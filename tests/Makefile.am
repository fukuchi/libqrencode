if HAVE_SDL
sdlPROGRAMS = view_qrcode
endif
noinst_PROGRAMS = test_qrinput test_bitstream test_estimatebit \
				  test_qrspec test_rs test_qrencode $(sdlPROGRAMS)

test_qrinput_SOURCES = test_qrinput.c
test_qrinput_LDADD = ../libqrencode.la

test_bitstream_SOURCES = test_bitstream.c
test_bitstream_LDADD = ../bitstream.o

test_estimatebit_SOURCES = test_estimatebit.c
test_estimatebit_LDADD = ../libqrencode.la

test_qrspec_SOURCES = test_qrspec.c
test_qrspec_LDADD = ../qrspec.o

test_rs_SOURCES = test_rs.c
test_rs_LDADD = ../libqrencode.la

test_qrencode_SOURCES = test_qrencode.c
test_qrencode_LDADD = ../libqrencode.la

if HAVE_SDL
view_qrcode_SOURCES = view_qrcode.c
view_qrcode_CFLAGS= $(SDL_CFLAGS)
view_qrcode_LDADD = ../libqrencode.la $(SDL_LIBS)
endif
