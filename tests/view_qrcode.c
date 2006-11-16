#include <stdio.h>
#include <string.h>
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
	QRenc_DataStream *stream;
	char num[9] = "01234567";
	unsigned char *frame, *q;
	unsigned int v, *p1, *p2;
	int width;
	int x, y;
	int pitch;
	QRcode *qrcode;

	stream = QRenc_newData();
	QRenc_setVersion(stream, 1);
	QRenc_setErrorCorrectionLevel(stream, QR_EC_LEVEL_L);
	QRenc_appendData(stream, QR_MODE_NUM, 8, (unsigned char *)num);
	qrcode = QRenc_encode(stream);
	width = qrcode->width;
	frame = qrcode->data;
	QRenc_freeData(stream);
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
	eventloop();


	QRenc_freeQRcode(qrcode);
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
