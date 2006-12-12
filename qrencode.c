/*
 * qrencode - QR Code encoder
 *
 * Copyright (C) 2006 Kentaro Fukuchi <fukuchi@megaui.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

QRRawCode *QRraw_new(QRinput *input)
{
	QRRawCode *raw;
	int *spec;
	int i;
	RSblock *rsblock;
	unsigned char *p;

	p = QRinput_getByteStream(input);
	if(p == NULL) {
		return NULL;
	}

	raw = (QRRawCode *)malloc(sizeof(QRRawCode));
	raw->datacode = p;
	spec = QRspec_getEccSpec(input->version, input->level);
	raw->version = input->version;
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

unsigned char *QRinput_fillerTest(int version)
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

void QRinput_writeFormatInformation(int width, unsigned char *frame, int mask, QRecLevel level)
{
	unsigned int format;
	unsigned char v;
	int i;

	format =  QRspec_getFormatInfo(mask, level);

	for(i=0; i<8; i++) {
		v = (unsigned char)(format & 1) | 0x84;
		frame[width * 8 + width - 1 - i] = v;
		if(i < 6) {
			frame[width * i + 8] = v;
		} else {
			frame[width * (i + 1) + 8] = v;
		}
		format= format >> 1;
	}
	for(i=0; i<7; i++) {
		v = (unsigned char)(format & 1) | 0x84;
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
			if(*s & 0x80) {\
				*d = *s;\
			} else {\
				*d = *s ^ ((__exp__) == 0);\
			}\
			b += (*d & 1);\
			s++; d++;\
		}\
	}\
	return b;

static int QRinput_mask0(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((x+y)&1)
}

static int QRinput_mask1(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(y&1)
}

static int QRinput_mask2(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(x%3)
}

static int QRinput_mask3(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((x+y)%3)
}

static int QRinput_mask4(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(((y/2)+(x/3))&1)
}

static int QRinput_mask5(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(((x*y)&1)+(x*y)%3)
}

static int QRinput_mask6(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((((x*y)&1)+(x*y)%3)&1)
}

static int QRinput_mask7(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((((x*y)%3)+((x+y)&1))&1)
}

typedef int MaskMaker(int, const unsigned char *, unsigned char *);
static MaskMaker *maskMakers[] = {
	QRinput_mask0, QRinput_mask1, QRinput_mask2, QRinput_mask3,
	QRinput_mask4, QRinput_mask5, QRinput_mask6, QRinput_mask7
};

unsigned char *QRinput_makeMask(int width, unsigned char *frame, int mask)
{
	unsigned char *masked;

	masked = (unsigned char *)malloc(width * width);

	maskMakers[mask](width, frame, masked);

	return masked;
}

static int runLength[QRSPEC_WIDTH_MAX + 1];

//static int n1;
//static int n2;
//static int n3;
//static int n4;

static int QRinput_calcN1N3(int length, int *runLength)
{
	int i;
	int demerit = 0;
	int fact;

	for(i=0; i<length; i++) {
		if(runLength[i] >= 5) {
			demerit += N1 + (runLength[i] - 5);
			//n1 += N1 + (runLength[i] - 5);
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
						//n3 += N3;
					} else if(i+3 >= length || runLength[i+3] >= 4 * fact) {
						demerit += N3;
						//n3 += N3;
					}
				}
			}
		}
	}

	return demerit;
}

int QRinput_evaluateSymbol(int width, unsigned char *frame)
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
				b22 = p[0] & p[-1] & p[-width] & p [-width-1];
				w22 = p[0] | p[-1] | p[-width] | p [-width-1];
				if((b22 | (w22 ^ 1))&1) {
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
		demerit += QRinput_calcN1N3(head+1, runLength);
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
		demerit += QRinput_calcN1N3(head+1, runLength);
	}

	return demerit;
}

static unsigned char *QRinput_mask(int width, unsigned char *frame, QRecLevel level)
{
	int i;
	unsigned char *mask, *bestMask;
	int minDemerit = INT_MAX;
	int bestMaskNum = 0;
	int blacks;
	int demerit;

	bestMask = NULL;

	for(i=0; i<8; i++) {
//		n1 = n2 = n3 = n4 = 0;
		demerit = 0;
		mask = (unsigned char *)malloc(width * width);
		blacks = maskMakers[i](width, frame, mask);
		blacks = 100 * blacks / (width * width);
		demerit = (abs(blacks - 50) / 5) * N4;
//		n4 = demerit;
		if(demerit > minDemerit) {
			free(mask);
			continue;
		}
		demerit += QRinput_evaluateSymbol(width, mask);
//		printf("(%d,%d,%d,%d)=%d\n", n1, n2, n3 ,n4, demerit);
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

	QRinput_writeFormatInformation(width, bestMask, bestMaskNum, level);

	return bestMask;
}

/******************************************************************************
 * QR-code encoding
 *****************************************************************************/

static QRcode *QRcode_new(int version, int width, unsigned char *data)
{
	QRcode *qrcode;

	qrcode = (QRcode *)malloc(sizeof(QRcode));
	qrcode->version = version;
	qrcode->width = width;
	qrcode->data = data;

	return qrcode;
}

void QRcode_free(QRcode *qrcode)
{
	if(qrcode == NULL) return;

	if(qrcode->data != NULL) {
		free(qrcode->data);
	}
	free(qrcode);
}

QRcode *QRcode_encodeInput(QRinput *input, int version, QRecLevel level)
{
	return QRcode_encodeMask(input, version, level, -1);
}

QRcode *QRcode_encodeMask(QRinput *input, int version, QRecLevel level, int mask)
{
	int width;
	QRRawCode *raw;
	unsigned char *frame, *masked, *p, code, bit;
	FrameFiller *filler;
	int i, j;
	QRcode *qrcode;

	QRinput_setVersion(input, version);
	QRinput_setErrorCorrectionLevel(input, level);

	raw = QRraw_new(input);
	if(raw == NULL) {
		return NULL;
	}
	version = raw->version;
	width = QRspec_getWidth(version);
	frame = QRspec_newFrame(version);
	filler = FrameFiller_new(width, frame);

	/* inteleaved data and ecc codes */
	for(i=0; i<raw->dataLength + raw->eccLength; i++) {
		code = QRraw_getCode(raw);
		bit = 0x80;
		for(j=0; j<8; j++) {
			p = FrameFiller_next(filler);
			*p = 0x02 | ((bit & code) != 0);
			bit = bit >> 1;
		}
	}
	QRraw_free(raw);
	/* remainder bits */
	j = QRspec_getRemainder(version);
	for(i=0; i<j; i++) {
		p = FrameFiller_next(filler);
		*p = 0x02;
	}
	free(filler);
	/* masking */
	if(mask < 0) {
		masked = QRinput_mask(width, frame, level);
	} else {
		masked = (unsigned char *)malloc(width * width);
		maskMakers[mask](width, frame, masked);
		QRinput_writeFormatInformation(width, masked, mask, QRinput_getErrorCorrectionLevel(input));
	}
	qrcode = QRcode_new(version, width, masked);

	free(frame);

	return qrcode;
}

static int QRcode_eatNum(const char *string, QRinput *input, int version, QRencodeMode hint);
static int QRcode_eatAn(const char *string, QRinput *input, int version, QRencodeMode hint);
static int QRcode_eat8(const char *string, QRinput *input, int version, QRencodeMode hint);
static int QRcode_eatKanji(const char *string, QRinput *input, int version, QRencodeMode hint);

#define isdigit(__c__) ((unsigned char)((signed char)(__c__) - '0') < 10)
#define isalnum(__c__) (QRinput_lookAnTable(__c__) >= 0)

static int QRcode_eatNum(const char *string, QRinput *input, int version, QRencodeMode hint)
{
	const char *p;
	int run;
	int dif;
	int ln;

	ln = QRspec_lengthIndicator(QR_MODE_NUM, version);

	p = string;
	while(isdigit(*p)) {
		p++;
	}
	run = p - string;
	if(*p & 0x80) {
		dif = QRinput_estimateBitsModeNum(run) + 4 + ln
			+ QRinput_estimateBitsMode8(1) /* + 4 + l8 */
			- QRinput_estimateBitsMode8(run + 1) /* - 4 - l8 */;
		if(dif > 0) {
			return QRcode_eat8(string, input, version, hint);
		}
	}
	if(isalnum(*p)) {
		dif = QRinput_estimateBitsModeNum(run) + 4 + ln
			+ QRinput_estimateBitsModeAn(1) /* + 4 + la */
			- QRinput_estimateBitsModeAn(run + 1) /* - 4 - la */;
		if(dif > 0) {
			return QRcode_eatAn(string, input, version, hint);
		}
	}

	QRinput_append(input, QR_MODE_NUM, run, (unsigned char *)string);
	return run;
}

static int QRcode_eatAn(const char *string, QRinput *input, int version, QRencodeMode hint)
{
	const char *p, *q;
	int run;
	int dif;
	int la, ln;

	la = QRspec_lengthIndicator(QR_MODE_AN, version);
	ln = QRspec_lengthIndicator(QR_MODE_NUM, version);

	p = string;
	while(isalnum(*p)) {
		if(isdigit(*p)) {
			q = p;
			while(isdigit(*q)) {
				q++;
			}
			dif = QRinput_estimateBitsModeAn(p - string) /* + 4 + la */
				+ QRinput_estimateBitsModeNum(q - p) + 4 + ln
				- QRinput_estimateBitsModeAn(q - string) /* - 4 - la */;
			if(dif < 0) {
				break;
			} else {
				p = q;
			}
		} else {
			p++;
		}
	}

	run = p - string;

	if(*p & 0x80) {
		dif = QRinput_estimateBitsModeAn(run) + 4 + la
			+ QRinput_estimateBitsMode8(1) /* + 4 + l8 */
			- QRinput_estimateBitsMode8(run + 1) /* - 4 - l8 */;
		if(dif > 0) {
			return QRcode_eat8(string, input, version, hint);
		}
	}

	QRinput_append(input, QR_MODE_AN, run, (unsigned char *)string);
	return run;
}

static int QRcode_eatKanji(const char *string, QRinput *input, int version, QRencodeMode hint)
{
	const char *p;

	p = string;
	while(QRinput_identifyMode(p) == QR_MODE_KANJI) {
		p += 2;
	}
	QRinput_append(input, QR_MODE_KANJI, p - string, (unsigned char *)string);
	return p - string;
}

static int QRcode_eat8(const char *string, QRinput *input, int version, QRencodeMode hint)
{
	const char *p, *q;
	QRencodeMode mode;
	int dif;
	int la, ln;

	la = QRspec_lengthIndicator(QR_MODE_AN, version);
	ln = QRspec_lengthIndicator(QR_MODE_NUM, version);

	p = string;
	while(*p != '\0') {
		mode = QRinput_identifyMode(p);
		if(hint == QR_MODE_KANJI && mode == QR_MODE_KANJI) {
			break;
		}
		if(mode != QR_MODE_8 && mode != QR_MODE_KANJI) {
			if(mode == QR_MODE_NUM) {
				q = p;
				while(isdigit(*q)) {
					q++;
				}
				dif = QRinput_estimateBitsMode8(p - string) /* + 4 + l8 */
					+ QRinput_estimateBitsModeNum(q - p) + 4 + ln
					- QRinput_estimateBitsMode8(q - string) /* - 4 - l8 */;
				if(dif < 0) {
					break;
				} else {
					p = q;
				}
			} else {
				q = p;
				while(isalnum(*q)) {
					q++;
				}
				dif = QRinput_estimateBitsMode8(p - string) /* + 4 + l8 */
					+ QRinput_estimateBitsModeAn(q - p) + 4 + la
					- QRinput_estimateBitsMode8(q - string) /* - 4 - l8 */;
				if(dif < 0) {
					break;
				} else {
					p = q;
				}
			}
		} else {
			p++;
		}
	}

	QRinput_append(input, QR_MODE_8, p - string, (unsigned char *)string);
	return p - string;
}

void QRcode_splitStringToQRinput(const char *string, QRinput *input,
		int version, QRencodeMode hint)
{
	int length;
	QRencodeMode mode;

	if(*string == '\0') return;

	mode = QRinput_identifyMode(string);
	if(mode == QR_MODE_NUM) {
		length = QRcode_eatNum(string, input, version, hint);
	} else if(mode == QR_MODE_AN) {
		length = QRcode_eatAn(string, input, version, hint);
	} else if(mode == QR_MODE_KANJI && hint == QR_MODE_KANJI) {
		length = QRcode_eatKanji(string, input, version, hint);
	} else {
		length = QRcode_eat8(string, input, version, hint);
	}
	if(length == 0) return;
	/* Of course this tail recursion could be optimized! Believe gcc. */
	QRcode_splitStringToQRinput(&string[length], input, hint, version);
}

QRcode *QRcode_encodeString(const char *string, int version, QRecLevel level, QRencodeMode hint)
{
	QRinput *input;
	QRcode *code;

	if(hint != QR_MODE_8 && hint != QR_MODE_KANJI) {
		return NULL;
	}

	input = QRinput_new();
	QRcode_splitStringToQRinput(string, input, version, hint);
	code = QRcode_encodeInput(input, version, level);
	QRinput_free(input);

	return code;
}
