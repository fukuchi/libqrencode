if HAVE_SDL
sdlPROGRAMS = view_qrcode
endif

noinst_PROGRAMS = test_qrinput test_bitstream test_estimatebit \
				  test_qrspec test_rs test_qrencode prof_qrencode \
				  test_mqrspec test_split test_monkey test_mask test_mmask \
				  create_frame_pattern create_mqr_frame_pattern \
				  $(sdlPROGRAMS)
noinst_LIBRARIES = libdecoder.a
DECODER_LIBS = libdecoder.a $(LIBICONV)
noinst_HEADERS = common.h
if HAVE_LIBPTHREAD
noinst_PROGRAMS += pthread_qrencode
endif

EXTRA_DIST = frame

libdecoder_a_SOURCES = decoder.c decoder.h

test_qrinput_SOURCES = test_qrinput.c
test_qrinput_LDADD = ../libqrencode.la $(DECODER_LIBS)

test_bitstream_SOURCES = test_bitstream.c
test_bitstream_LDADD = ../libqrencode.la

test_estimatebit_SOURCES = test_estimatebit.c
test_estimatebit_LDADD = ../libqrencode.la

test_qrspec_SOURCES = test_qrspec.c
test_qrspec_LDADD = ../libqrencode.la $(DECODER_LIBS)

test_mqrspec_SOURCES = test_mqrspec.c
test_mqrspec_LDADD = ../libqrencode.la $(DECODER_LIBS)

test_rs_SOURCES = test_rs.c
test_rs_LDADD = ../libqrencode.la

test_qrencode_SOURCES = test_qrencode.c
test_qrencode_LDADD = ../libqrencode.la $(DECODER_LIBS)

test_split_SOURCES = test_split.c
test_split_LDADD = ../libqrencode.la

test_monkey_SOURCES = test_monkey.c
test_monkey_LDADD = ../libqrencode.la $(DECODER_LIBS)

test_mask_SOURCES = test_mask.c
test_mask_LDADD = ../libqrencode.la $(DECODER_LIBS)

test_mmask_SOURCES = test_mmask.c
test_mmask_LDADD = ../libqrencode.la $(DECODER_LIBS)

prof_qrencode_SOURCES = prof_qrencode.c
prof_qrencode_LDADD = ../libqrencode.la

pthread_qrencode_SOURCES = pthread_qrencode.c
pthread_qrencode_LDADD = ../libqrencode.la $(LIBPTHREAD)

create_frame_pattern_SOURCES = create_frame_pattern.c
create_frame_pattern_CFLAGS = $(png_CFLAGS)
create_frame_pattern_LDADD = ../libqrencode.la $(png_LIBS)

create_mqr_frame_pattern_SOURCES = create_mqr_frame_pattern.c
create_mqr_frame_pattern_CFLAGS = $(png_CFLAGS)
create_mqr_frame_pattern_LDADD = ../libqrencode.la $(png_LIBS)

if HAVE_SDL
view_qrcode_SOURCES = view_qrcode.c
view_qrcode_CFLAGS= $(SDL_CFLAGS)
view_qrcode_LDADD = ../libqrencode.la $(SDL_LIBS)
endif
