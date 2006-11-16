#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <SDL.h>
#include "common.h"
#include "../qrencode_inner.h"
#include "../qrspec.h"

SDL_Surface *screen = NULL;
#define WIDTH 400

int eventloop(void)
{
	SDL_Event event;
	int q = 1;

	while(q) {
		usleep(10000);
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
				case SDLK_SPACE:
					q = 0;
					break;
				case SDLK_ESCAPE:
					q = 0;
					break;
				default:
					break;
				}
			}
			if(event.type == SDL_QUIT) {
				return -1;
			}
		}
	}

	return 0;
}

void view_simple(void)
{
	QRinput *stream;
	char num[9] = "01234567";
	unsigned char *frame, *q;
	unsigned int v, *p1, *p2;
	int width;
	int x, y;
	int pitch;
	int flag = 1;
	int version = 1;
	int mask = 0;
	QRecLevel level = QR_ECLEVEL_L;
	QRcode *qrcode;
	SDL_Event event;
	int loop;

	stream = QRenc_newData();

	QRenc_appendData(stream, QR_MODE_NUM, 8, (unsigned char *)num);

	while(flag) {
		QRenc_setVersion(stream, version);
		QRenc_setErrorCorrectionLevel(stream, level);
		qrcode = QRenc_encodeMask(stream, mask);
		width = qrcode->width;
		frame = qrcode->data;
		pitch = screen->pitch;
		q = frame;
		SDL_FillRect(screen, NULL, 0xffffff);
		for(y=0; y<width; y++) {
			p1 = (unsigned int *)(screen->pixels + pitch * (y + 4) * 2 + 32);
			p2 = (unsigned int *)(screen->pixels + pitch * ((y + 4) * 2 + 1) + 32);
			for(x=0; x<width; x++) {
				v = (*q&1)?0:0xffffff;
				p1[x * 2] = v;
				p1[x * 2 + 1] = v;
				p2[x * 2] = v;
				p2[x * 2 + 1] = v;
				q++;
			}
		}
		SDL_Flip(screen);
		QRenc_freeQRcode(qrcode);
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

	QRenc_freeData(stream);
}

int main()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed initializing SDL: %s\n", SDL_GetError());
		return -1;
	}

	screen = SDL_SetVideoMode(WIDTH, WIDTH, 32, 0);

	view_simple();

	return 0;
}
