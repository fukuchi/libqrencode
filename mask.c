/*
 * qrencode - QR Code encoder
 *
 * Masking.
 * Copyright (C) 2006, 2007, 2008 Kentaro Fukuchi <fukuchi@megaui.net>
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

#include <stdlib.h>
#include <limits.h>
#include "qrencode.h"
#include "qrencode_inner.h"
#include "qrspec.h"
#include "mask.h"

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

static int Mask_mask0(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((x+y)&1)
}

static int Mask_mask1(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(y&1)
}

static int Mask_mask2(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(x%3)
}

static int Mask_mask3(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((x+y)%3)
}

static int Mask_mask4(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(((y/2)+(x/3))&1)
}

static int Mask_mask5(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER(((x*y)&1)+(x*y)%3)
}

static int Mask_mask6(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((((x*y)&1)+(x*y)%3)&1)
}

static int Mask_mask7(int width, const unsigned char *s, unsigned char *d)
{
	MASKMAKER((((x*y)%3)+((x+y)&1))&1)
}

typedef int MaskMaker(int, const unsigned char *, unsigned char *);
static MaskMaker *maskMakers[] = {
	Mask_mask0, Mask_mask1, Mask_mask2, Mask_mask3,
	Mask_mask4, Mask_mask5, Mask_mask6, Mask_mask7
};

unsigned char *Mask_makeMask(int width, unsigned char *frame, int mask)
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

static int Mask_calcN1N3(int length, int *runLength)
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

int Mask_evaluateSymbol(int width, unsigned char *frame)
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
		demerit += Mask_calcN1N3(head+1, runLength);
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
		demerit += Mask_calcN1N3(head+1, runLength);
	}

	return demerit;
}

unsigned char *Mask_mask(int width, unsigned char *frame, QRecLevel level)
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
		blacks += QRcode_writeFormatInformation(width, mask, i, level);
		blacks = 100 * blacks / (width * width);
		demerit = (abs(blacks - 50) / 5) * N4;
//		n4 = demerit;
		demerit += Mask_evaluateSymbol(width, mask);
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

	return bestMask;
}
