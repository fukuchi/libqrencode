#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <SDL.h>
#include <getopt.h>
#include "common.h"
#include "../qrencode_inner.h"
#include "../qrspec.h"
#include "../qrinput.h"
#include "../split.h"

static SDL_Surface *screen = NULL;
static int casesensitive = 1;
static int eightbit = 0;
static int version = 1;
static int size = 4;
static int margin = 4;
static QRecLevel level = QR_ECLEVEL_L;
static QRencodeMode hint = QR_MODE_8;

enum {
	O_HELP,
	O_SIZE,
	O_VERSION,
	O_LEVEL,
	O_MARGIN,
	O_KANJI,
	O_CASE,
	O_IGNORECASE,
	O_8BIT,
};

const struct option options[] = {
	{"h", no_argument      , NULL, O_HELP},
	{"l", required_argument, NULL, O_LEVEL},
	{"s", required_argument, NULL, O_SIZE},
	{"v", required_argument, NULL, O_VERSION},
	{"m", required_argument, NULL, O_MARGIN},
	{"k", no_argument      , NULL, O_KANJI},
	{"c", no_argument      , NULL, O_CASE},
	{"i", no_argument      , NULL, O_IGNORECASE},
	{"8", no_argument      , NULL, O_8BIT},
	{NULL, 0, NULL, 0}
};


static char levelChar[4] = {'L', 'M', 'Q', 'H'};
static void usage(void)
{
	fprintf(stderr,
"view_qrcode version %s\n"
"Copyright (C) 2008 Kentaro Fukuchi\n"
"Usage: view_qrcode [OPTION]... [STRING]\n"
"Encode input data in a QR Code and save as a PNG image.\n\n"
"  -h           display this message.\n"
"  -s NUMBER    specify the size of dot (pixel). (default=4)\n"
"  -l {LMQH}    specify error collectin level from L (lowest) to H (highest).\n"
"               (default=L)\n"
"  -v NUMBER    specify the version of the symbol. (default=auto)\n"
"  -m NUMBER    specify the width of margin. (default=4)\n"
"  -k           assume that the input text contains kanji (shift-jis).\n"
"  -c           encode lower-case alphabet characters in 8-bit mode. (default)\n"
"  -i           ignore case distinctions and use only upper-case characters.\n"
"  -8           encode entire data in 8-bit mode. -k, -c and -i will be ignored.\n"
"  [STRING]     input data. If it is not specified, data will be taken from\n"
"               standard input.\n",
	VERSION);
}

#define MAX_DATA_SIZE 7090 /* from the specification */
static char *readStdin(void)
{
	char *buffer;
	int ret;

	buffer = (char *)malloc(MAX_DATA_SIZE);
	ret = fread(buffer, 1, MAX_DATA_SIZE, stdin);
	if(ret == 0) {
		fprintf(stderr, "No input data.\n");
		exit(1);
	}
	if(!feof(stdin)) {
		fprintf(stderr, "Input data is too large.\n");
		exit(1);
	}

	buffer[ret] = '\0';

	return buffer;
}

void view_simple(const char *str)
{
	QRinput *stream;
	unsigned char *frame, *q;
	int width;
	int x, y;
	int pitch;
	int flag = 1;
	int mask = -1;
	QRcode *qrcode;
	SDL_Event event;
	int loop;
	SDL_Rect rect;

	stream = QRinput_new();
	if(eightbit) {
		QRinput_append(stream, QR_MODE_8, strlen(str), (unsigned char *)str);
	} else {
		Split_splitStringToQRinput(str, stream, 0, QR_MODE_KANJI, casesensitive);
	}

	while(flag) {
		qrcode = QRcode_encodeMask(stream, version, level, mask);
		width = qrcode->width;
		frame = qrcode->data;
		version = qrcode->version;
		printf("Version %d, Leve %c, Mask %d.\n", version, levelChar[level], mask);
		screen = SDL_SetVideoMode((width + margin*2) * size, (width + margin*2) * size, 32, 0);
		pitch = screen->pitch;
		q = frame;
		SDL_FillRect(screen, NULL, 0xffffff);
		for(y=0; y<width; y++) {
			for(x=0; x<width; x++) {
				rect.x = (margin + x) * size;
				rect.y = (margin + y) * size;
				rect.w = size;
				rect.h = size;
				SDL_FillRect(screen, &rect, (*q&1)?0:0xffffff);
				q++;
			}
		}
		SDL_Flip(screen);
		QRcode_free(qrcode);
		loop = 1;
		while(loop) {
			usleep(10000);
			while(SDL_PollEvent(&event)) {
				if(event.type == SDL_KEYDOWN) {
					switch(event.key.keysym.sym) {
					case SDLK_RIGHT:
						version++;
						if(version > QRSPEC_VERSION_MAX)
							version = QRSPEC_VERSION_MAX;
						loop = 0;
						break;
					case SDLK_LEFT:
						version--;
						if(version < 1)
							version = 1;
						loop = 0;
						break;
					case SDLK_UP:
						size++;
						loop = 0;
						break;
					case SDLK_DOWN:
						size--;
						if(size < 1) size = 1;
						loop = 0;
						break;
					case SDLK_0:
					case SDLK_1:
					case SDLK_2:
					case SDLK_3:
					case SDLK_4:
					case SDLK_5:
					case SDLK_6:
					case SDLK_7:
						mask = (event.key.keysym.sym - SDLK_0);
						loop = 0;
						break;
					case SDLK_8:
						mask = -1;
						loop = 0;
						break;
					case SDLK_l:
						level = QR_ECLEVEL_L;
						loop = 0;
						break;
					case SDLK_m:
						level = QR_ECLEVEL_M;
						loop = 0;
						break;
					case SDLK_h:
						level = QR_ECLEVEL_H;
						loop = 0;
						break;
					case SDLK_q:
						level = QR_ECLEVEL_Q;
						loop = 0;
						break;
					case SDLK_ESCAPE:
						loop = 0;
						flag = 0;
						break;
					default:
						break;
					}
				}
				if(event.type == SDL_QUIT) {
					loop = 0;
					flag = 0;
				}
			}
		}
	}

	QRinput_free(stream);
}

int main(int argc, char **argv)
{
	int opt;
	char *intext = NULL;

	while((opt = getopt_long_only(argc, argv, "", options, NULL)) != -1) {
		switch(opt) {
			case O_HELP:
				usage();
				exit(0);
				break;
			case O_SIZE:
				size = atoi(optarg);
				if(size <= 0) {
					fprintf(stderr, "Invalid size: %d\n", size);
					exit(1);
				}
				break;
			case O_VERSION:
				version = atoi(optarg);
				if(version < 0) {
					fprintf(stderr, "Invalid version: %d\n", version);
					exit(1);
				}
				break;
			case O_LEVEL:
				switch(*optarg) {
					case 'l':
					case 'L':
						level = QR_ECLEVEL_L;
						break;
					case 'm':
					case 'M':
						level = QR_ECLEVEL_M;
						break;
					case 'q':
					case 'Q':
						level = QR_ECLEVEL_Q;
						break;
					case 'h':
					case 'H':
						level = QR_ECLEVEL_H;
						break;
					default:
						fprintf(stderr, "Invalid level: %s\n", optarg);
						exit(1);
						break;
				}
				break;
			case O_MARGIN:
				margin = atoi(optarg);
				if(margin < 0) {
					fprintf(stderr, "Invalid margin: %d\n", margin);
					exit(1);
				}
				break;
			case O_KANJI:
				hint = QR_MODE_KANJI;
				break;
			case O_CASE:
				casesensitive = 1;
				break;
			case O_IGNORECASE:
				casesensitive = 0;
				break;
			case O_8BIT:
				eightbit = 1;
				break;
			default:
				usage();
				exit(1);
				break;
		}
	}
	if(argc == 1) {
		usage();
		exit(0);
	}

	if(optind < argc) {
		intext = argv[optind];
	}
	if(intext == NULL) {
		intext = readStdin();
	}

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed initializing SDL: %s\n", SDL_GetError());
		return -1;
	}
	view_simple(intext);

	SDL_Quit();

	return 0;
}
