/**
 * qrencode - QR-code encoder
 *
 * Copyright (C) 2006 Kentaro Fukuchi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "qrencode.h"
#include "qrencode_inner.h"
#include "qrspec.h"
#include "bitstream.h"
#include "qrinput.h"
#include "rscode.h"

/******************************************************************************
 * Raw code
 *****************************************************************************/

static void RSblock_init(RSblock *block, int dl, unsigned char *data, int el)
{
	RS *rs;

	block->dataLength = dl;
	block->data = data;
	block->eccLength = el;
	block->ecc = (unsigned char *)malloc(el);

	rs = init_rs(8, 0x11d, 0, 1, el, 255 - dl - el);
	encode_rs_char(rs, data, block->ecc);
}

QRRawCode *QRraw_new(QRinput *stream)
{
	QRRawCode *raw;
	int *spec;
	int i;
	RSblock *rsblock;
	unsigned char *p;

	raw = (QRRawCode *)malloc(sizeof(QRRawCode));
	raw->datacode = QRenc_getByteStream(stream);
	spec = QRspec_getEccSpec(stream->version, stream->level);
	raw->blocks = QRspec_rsBlockNum(spec);
	raw->rsblock = (RSblock *)malloc(sizeof(RSblock) * raw->blocks);

	rsblock = raw->rsblock;
	p = raw->datacode;
	for(i=0; i<QRspec_rsBlockNum1(spec); i++) {
		RSblock_init(rsblock, QRspec_rsDataCodes1(spec), p,
						QRspec_rsEccCodes1(spec));
		p += QRspec_rsDataCodes1(spec);
		rsblock++;
	}
	for(i=0; i<QRspec_rsBlockNum2(spec); i++) {
		RSblock_init(rsblock, QRspec_rsDataCodes2(spec), p,
						QRspec_rsEccCodes2(spec));
		p += QRspec_rsDataCodes2(spec);
		rsblock++;
	}

	raw->b1 = QRspec_rsBlockNum1(spec);
	raw->b2 = QRspec_rsBlockNum2(spec);
	raw->dataLength = QRspec_rsBlockNum1(spec) * QRspec_rsDataCodes1(spec)
					+ QRspec_rsBlockNum2(spec) * QRspec_rsDataCodes2(spec);
	raw->eccLength = QRspec_rsBlockNum(spec) * QRspec_rsEccCodes1(spec);
	raw->count = 0;

	free(spec);

	return raw;
}

/**
 * Return a code (byte).
 * This function can be called iteratively.
 * @param raw raw code.
 * @return code
 */
unsigned char QRraw_getCode(QRRawCode *raw)
{
	int col, row;
	unsigned char ret;

	if(raw->count < raw->dataLength) {
		row = raw->count % raw->blocks;
		col = raw->count / raw->blocks;
		if(col >= raw->rsblock[row].dataLength) {
			row += raw->b1;
		}
		ret = raw->rsblock[row].data[col];
	} else if(raw->count < raw->dataLength + raw->eccLength) {
		row = (raw->count - raw->dataLength) % raw->blocks;
		col = (raw->count - raw->dataLength) / raw->blocks;
		ret = raw->rsblock[row].ecc[col];
	} else {
		return 0;
	}
	raw->count++;
	return ret;
}

void QRraw_free(QRRawCode *raw)
{
	int i;

	free(raw->datacode);
	for(i=0; i<raw->blocks; i++) {
		free(raw->rsblock[i].ecc);
	}
	free(raw->rsblock);
	free(raw);
}

/******************************************************************************
 * Frame filling
 *****************************************************************************/

typedef struct {
	int width;
	unsigned char *frame;
	int x, y;
	int dir;
	int bit;
} FrameFiller;

static FrameFiller *FrameFiller_new(int width, unsigned char *frame)
{
	FrameFiller *filler;

	filler = (FrameFiller *)malloc(sizeof(FrameFiller));
	filler->width = width;
	filler->frame = frame;
	filler->x = width - 1;
	filler->y = width - 1;
	filler->dir = -1;
	filler->bit = -1;

	return filler;
}

static unsigned char *FrameFiller_next(FrameFiller *filler)
{
	unsigned char *p;
	int x, y, w;

	if(filler->bit == -1) {
		filler->bit = 0;
		return filler->frame + filler->y * filler->width + filler->x;
	}

	x = filler->x;
	y = filler->y;
	p = filler->frame;
	w = filler->width;

	if(filler->bit == 0) {
		x--;
		filler->bit++;
	} else {
		x++;
		y += filler->dir;
		filler->bit--;
	}

	if(filler->dir < 0) {
		if(y < 0) {
			y = 0;
			x -= 2;
			filler->dir = 1;
			if(x == 6) {
				x--;
				y = 9;
			}
		}
	} else {
		if(y == w) {
			y = w - 1;
			x -= 2;
			filler->dir = -1;
			if(x == 6) {
				x--;
				y -= 8;
			}
		}
	}
	if(x < 0 || y < 0) return NULL;

	filler->x = x;
	filler->y = y;

	if(p[y * w + x] & 0x80) {
		// This tail recursion could be optimized.
		return FrameFiller_next(filler);
	}
	return &p[y * w + x];
}

unsigned char *QRenc_fillerTest(int version)
{
	int width, length;
	unsigned char *frame, *p;
	FrameFiller *filler;
	int i, j;
	unsigned char cl = 1;
	unsigned char ch = 0;

	width = QRspec_getWidth(version);
	frame = QRspec_newFrame(version);
	filler = FrameFiller_new(width, frame);
	length = QRspec_getDataLength(version, QR_ECLEVEL_L)
			+ QRspec_getECCLength(version, QR_ECLEVEL_L);

	for(i=0; i<length; i++) {
		for(j=0; j<8; j++) {
			p = FrameFiller_next(filler);
			*p = ch | cl;
			cl++;
			if(cl == 9) {
				cl = 1;
				ch += 0x10;
			}
		}
	}
	length = QRspec_getRemainder(version);
	for(i=0; i<length; i++) {
		p = FrameFiller_next(filler);
		*p = 0xa0;
	}
	p = FrameFiller_next(filler);
	free(filler);
	if(p != NULL) {
		return NULL;
	}

	return frame;
}

/******************************************************************************
 * Format information
 *****************************************************************************/

void QRenc_writeFormatInformation(int width, unsigned char *frame, int mask, QRecLevel level)
{
	unsigned int format;
	unsigned char v;
	int i;

	format =  QRspec_getFormatInfo(mask, level);

	for(i=0; i<8; i++) {
		v = (unsigned char)(format & 1);
		frame[width * 8 + width - 1 - i] = v;
		if(i < 6) {
			frame[width * i + 8] = v;
		} else {
			frame[width * (i + 1) + 8] = v;
		}
		format= format >> 1;
	}
	for(i=0; i<7; i++) {
		v = (unsigned char)(format & 1);
		frame[width * (width - 7 + i) + 8] = v;
		if(i == 0) {
			frame[width * 8 + 7] = v;
		} else {
			frame[width * 8 + 6 - i] = v;
		}
		format= format >> 1;
	}
}

/******************************************************************************
 * Masking
 *****************************************************************************/

/**
 * Demerit coefficients.
 * See Section 8.8.2, pp.45, JIS X0510:2004.
 */
#define N1 (3)
#define N2 (3)
#define N3 (40)
#define N4 (10)

#define MASKMAKER(__exp__) \
	int x, y;\
	unsigned int b = 0;\
\
	for(y=0; y<width; y++) {\
		for(x=0; x<width; x++) {\
			if(*s & 0x20) {\
				*d = *s ^ ((__exp__) == 0);\
			} else {\
				*d = *s;\
			}\
			b += (*d & 1);\
			s++; d++;\
		}\
	}\
	return b;

static int QRenc_mask0(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((x+y)&1)
}

static int QRenc_mask1(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(y&1)
}

static int QRenc_mask2(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(x%3)
}

static int QRenc_mask3(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((x+y)%3)
}

static int QRenc_mask4(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(((y/2)+(x/3))&1)
}

static int QRenc_mask5(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(((x*y)&1)+(x*y)%3)
}

static int QRenc_mask6(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((((x*y)&1)+(x*y)%3)&1)
}

static int QRenc_mask7(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((((x*y)%3)+((x+y)&1))&1)
}

typedef int MaskMaker(int, const unsigned char *, unsigned char *);
static MaskMaker *maskMakers[] = {
	QRenc_mask0, QRenc_mask1, QRenc_mask2, QRenc_mask3,
	QRenc_mask4, QRenc_mask5, QRenc_mask6, QRenc_mask7
};

unsigned char *QRenc_makeMask(int width, unsigned char *frame, int mask)
{
	unsigned char *masked;

	masked = (unsigned char *)malloc(width * width);

	maskMakers[mask](width, frame, masked);

	return masked;
}

static int runLength[QRSPEC_WIDTH_MAX + 1];

static int QRenC_calcN1N3(int length, int *runLength)
{
	int i;
	int demerit = 0;
	int fact;

	for(i=0; i<length; i++) {
		if(runLength[i] >= 5) {
			demerit += N1 + (runLength[i] - 5);
		}
		if((i & 1)) {
			if(i >= 3 && i < length-2 && (runLength[i] % 3) == 0) {
				fact = runLength[i] / 3;
				if(runLength[i-2] == fact &&
				   runLength[i-1] == fact &&
				   runLength[i+1] == fact &&
				   runLength[i+2] == fact) {
					if(runLength[i-3] < 0 || runLength[i-3] >= 4 * fact) {
						demerit += N3;
					} else if(i+3 >= length || runLength[i+3] >= 4 * fact) {
						demerit += N3;
					}
				}
			}
		}
	}

	return demerit;
}

int QRenc_evaluateSymbol(int width, unsigned char *frame)
{
	int x, y;
	unsigned char *p;
	unsigned char b22, w22;
	unsigned int i;
	int head;
	int demerit = 0;

	p = frame;
	i = 0;
	for(y=0; y<width; y++) {
		head = 0;
		runLength[0] = 1;
		for(x=0; x<width; x++) {
			if(x > 0 && y > 0) {
				b22 = p[0] & p[-1] & p[-width] & p [-width-1] & 1;
				w22 = (p[0] | p[-1] | p[-width] | p [-width-1] ) & 1;
				if(b22 | (w22 ^ 1)) {
					demerit += N2;
				}
			}
			if(x == 0 && (p[0] & 1)) {
				runLength[0] = -1;
				head = 1;
				runLength[head] = 1;
			} else if(x > 0) {
				if((p[0] ^ p[-1]) & 1) {
					head++;
					runLength[head] = 1;
				} else {
					runLength[head]++;
				}
			}
			p++;
		}
		demerit += QRenC_calcN1N3(head+1, runLength);
	}

	i = 0;
	for(x=0; x<width; x++) {
		head = 0;
		runLength[0] = 1;
		p = frame + x;
		for(y=0; y<width; y++) {
			if(y == 0 && (p[0] & 1)) {
				runLength[0] = -1;
				head = 1;
				runLength[head] = 1;
			} else if(y > 0) {
				if((p[0] ^ p[-width]) & 1) {
					head++;
					runLength[head] = 1;
				} else {
					runLength[head]++;
				}
			}
			p+=width;
		}
		demerit += QRenC_calcN1N3(head+1, runLength);
	}

	return demerit;
}

static unsigned char *QRenc_mask(int width, unsigned char *frame, QRecLevel level)
{
	int i;
	unsigned char *mask, *bestMask;
	int minDemerit = INT_MAX;
	int bestMaskNum = 0;
	int blacks;
	int demerit;

	bestMask = NULL;

	for(i=0; i<8; i++) {
		demerit = 0;
		mask = (unsigned char *)malloc(width * width);
		blacks = maskMakers[i](width, frame, mask);
		blacks = 100 * blacks / (width * width);
		demerit = (abs(blacks - 50) / 5) * N4;
		if(demerit > minDemerit) {
			free(mask);
			continue;
		}
		demerit += QRenc_evaluateSymbol(width, mask);
		if(demerit < minDemerit) {
			minDemerit = demerit;
			bestMaskNum = i;
			if(bestMask != NULL) {
				free(bestMask);
			}
			bestMask = mask;
		} else {
			free(mask);
		}
	}

	QRenc_writeFormatInformation(width, bestMask, bestMaskNum, level);

	return bestMask;
}

/******************************************************************************
 * QR-code encoding
 *****************************************************************************/

static QRcode *QRenc_newQRcode(int width, unsigned char *data)
{
	QRcode *qrcode;

	qrcode = (QRcode *)malloc(sizeof(QRcode));
	qrcode->width = width;
	qrcode->data = data;

	return qrcode;
}

void QRenc_freeQRcode(QRcode *qrcode)
{
	if(qrcode == NULL) return;

	if(qrcode->data != NULL) {
		free(qrcode->data);
	}
	free(qrcode);
}

QRcode *QRenc_encode(QRinput *stream)
{
	int version;
	int width;
	QRRawCode *raw;
	unsigned char *frame, *masked, *p, code, bit;
	FrameFiller *filler;
	int i, j;
	QRcode *qrcode;

	version = QRenc_getVersion(stream);
	width = QRspec_getWidth(version);
	raw = QRraw_new(stream);
	frame = QRspec_newFrame(version);
	filler = FrameFiller_new(width, frame);

	/* inteleaved data and ecc codes */
	for(i=0; i<raw->dataLength + raw->eccLength; i++) {
		code = QRraw_getCode(raw);
		bit = 0x80;
		for(j=0; j<8; j++) {
			p = FrameFiller_next(filler);
			*p = 0xa0 | ((bit & code) != 0);
			bit = bit >> 1;
		}
	}
	QRraw_free(raw);
	/* remainder bits */
	j = QRspec_getRemainder(version);
	for(i=0; i<j; i++) {
		p = FrameFiller_next(filler);
		*p = 0xa0;
	}
	free(filler);
	/* masking */
	masked = QRenc_mask(width, frame, QRenc_getErrorCorrectionLevel(stream));
	qrcode = QRenc_newQRcode(width, masked);

	free(frame);

	return qrcode;
}

QRcode *QRenc_encodeMask(QRinput *stream, int mask)
{
	int version;
	int width;
	QRRawCode *raw;
	unsigned char *frame, *masked, *p, code, bit;
	FrameFiller *filler;
	int i, j;
	QRcode *qrcode;

	version = QRenc_getVersion(stream);
	width = QRspec_getWidth(version);
	raw = QRraw_new(stream);
	frame = QRspec_newFrame(version);
	filler = FrameFiller_new(width, frame);

	/* inteleaved data and ecc codes */
	for(i=0; i<raw->dataLength + raw->eccLength; i++) {
		code = QRraw_getCode(raw);
		bit = 0x80;
		for(j=0; j<8; j++) {
			p = FrameFiller_next(filler);
			*p = 0xa0 | ((bit & code) != 0);
			bit = bit >> 1;
		}
	}
	QRraw_free(raw);
	/* remainder bits */
	j = QRspec_getRemainder(version);
	for(i=0; i<j; i++) {
		p = FrameFiller_next(filler);
		*p = 0xa0;
	}
	free(filler);
	/* masking */
	masked = (unsigned char *)malloc(width * width);
	maskMakers[mask](width, frame, masked);
	QRenc_writeFormatInformation(width, masked, mask, QRenc_getErrorCorrectionLevel(stream));
	qrcode = QRenc_newQRcode(width, masked);

	free(frame);

	return qrcode;
}
