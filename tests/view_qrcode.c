#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <SDL.h>
#include "common.h"
#include "../qrencode_inner.h"
#include "../qrspec.h"
#include "../qrinput.h"

SDL_Surface *screen = NULL;

void view_simple(const char *str)
{
	QRinput *stream;
	unsigned char *frame, *q;
	int width;
	int x, y;
	int pitch;
	int flag = 1;
	int version = 1;
	int mask = 0;
	int scale = 4;
	QRecLevel level = QR_ECLEVEL_L;
	QRcode *qrcode;
	SDL_Event event;
	int loop;
	SDL_Rect rect;

	stream = QRinput_new();
	QRcode_splitStringToQRinput(str, stream, 0, QR_MODE_KANJI);


	while(flag) {
		qrcode = QRcode_encodeMask(stream, version, level, mask);
		width = qrcode->width;
		frame = qrcode->data;
		version = qrcode->version;
		printf("Version %d, Leve %d, Mask %d.\n", version, level, mask);
		screen = SDL_SetVideoMode((width + 8) * scale, (width + 8) * scale, 32, 0);
		pitch = screen->pitch;
		q = frame;
		SDL_FillRect(screen, NULL, 0xffffff);
		for(y=0; y<width; y++) {
			for(x=0; x<width; x++) {
				rect.x = (4 + x) * scale;
				rect.y = (4 + y) * scale;
				rect.w = scale;
				rect.h = scale;
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
						scale++;
						loop = 0;
						break;
					case SDLK_DOWN:
						scale--;
						if(scale < 1) scale = 1;
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
	if(argc != 2) {
		printf("Usage: view_qrcode string\n");
		exit(1);
	}
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed initializing SDL: %s\n", SDL_GetError());
		return -1;
	}

	view_simple(argv[1]);

	SDL_Quit();

	return 0;
}
