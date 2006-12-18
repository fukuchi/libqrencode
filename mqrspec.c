/*
 * qrencode - QR Code encoder
 *
 * Micor QR Code specification in convenient format. 
 * Copyright (C) 2006 Kentaro Fukuchi <fukuchi@megaui.net>
 *
 * The following data / specifications are taken from
 * "Two dimensional symbol -- QR-code -- Basic Specification" (JIS X0510:2004)
 *  or
 * "Automatic identification and data capture techniques -- 
 *  QR Code 2005 bar code symbology specification" (ISO/IEC 18004:2006)
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

#include "mqrspec.h"

/******************************************************************************
 * Version and capacity
 *****************************************************************************/

typedef struct {
	int width; //< Edge length of the symbol
	int ec[3];  //< Number of ECC code (bytes)
} MQRspec_Capacity;

/**
 * Table of the capacity of symbols
 * See Table 1 (pp.10) and Table 8 (pp.113) of Appendix 1, JIS X0510:2004.
 */
static const MQRspec_Capacity mqrspecCapacity[MQRSPEC_VERSION_MAX + 1] = {
	{  0, {0,  0,  0}},
	{ 11, {2,  0,  0}},
	{ 13, {5,  6,  0}},
	{ 15, {6,  8,  0}},
	{ 17, {8, 10, 14}}
};

int MQRspec_getDataLength(int version, QRecLevel level)
{
	int w;
	int ecc;

	w = mqrspecCapacity[version].width - 1;
	ecc = mqrspecCapacity[version].ec[level];
	if(ecc == 0) return 0;

	return w * w - 64 - ecc * 8;
}

int MQRspec_getECCLength(int version, QRecLevel level)
{
	return mqrspecCapacity[version].ec[level];
}

int MQRspec_getWidth(int version)
{
	return mqrspecCapacity[version].width;
}

/******************************************************************************
 * Length indicator
 *****************************************************************************/

/**
 * See Table 3 (pp.107) of Appendix 1, JIS X0510:2004.
 */
static const int lengthTableBits[4][4] = {
	{ 3, 4, 5, 6},
	{ 0, 3, 4, 5},
	{ 0, 0, 4, 5},
	{ 0, 0, 3, 4}
};

int MQRspec_lengthIndicator(QRencodeMode mode, int version)
{
	return lengthTableBits[mode][version - 1];
}

int MQRspec_maximumWords(QRencodeMode mode, int version)
{
	int bits;
	int words;

	bits = lengthTableBits[mode][version - 1];
	words = (1 << bits) - 1;
	if(mode == QR_MODE_KANJI) {
		words *= 2; // the number of bytes is required
	}

	return words;
}

/******************************************************************************
 * Error correction code
 *****************************************************************************/

int *MQRspec_getEccSpec(int version, QRecLevel level)
{
#if 0
	int data, ecc;
	int *array;

	data = MQRspec_getDataLength(version, level);
	ecc = MQRspec_getECCLength(version, level);
#endif
	return NULL;
}

/******************************************************************************
 * Format information
 *****************************************************************************/

/* See calcFormatInfo in tests/test_mqrspec.c */
static const unsigned int const formatInfo[4][8] = {
	{0x4445, 0x55ae, 0x6793, 0x7678, 0x06de, 0x1735, 0x2508, 0x34e3},
	{0x4172, 0x5099, 0x62a4, 0x734f, 0x03e9, 0x1202, 0x203f, 0x31d4},
	{0x4e2b, 0x5fc0, 0x6dfd, 0x7c16, 0x0cb0, 0x1d5b, 0x2f66, 0x3e8d},
	{0x4b1c, 0x5af7, 0x68ca, 0x7921, 0x0987, 0x186c, 0x2a51, 0x3bba}
};

/* See Table 10 of Appendix 1. (pp.115) */
static const int const typeTable[MQRSPEC_VERSION_MAX + 1][3] = {
	{-1, -1, -1},
	{ 0, -1, -1},
	{ 1,  2, -1},
	{ 3,  4, -1},
	{ 5,  6,  7}
};

unsigned int MQRspec_getFormatInfo(int mask, int version, QRecLevel level)
{
	int type;

	if(mask < 0 || mask > 3) return 0;
	if(version <= 0 || version > MQRSPEC_VERSION_MAX) return 0;
	if(level == QR_ECLEVEL_H) return 0;
	type = typeTable[version][level];
	if(type < 0) return 0;

	return formatInfo[mask][type];
}

/******************************************************************************
 * Frame
 *****************************************************************************/

/**
 * Cache of initial frames.
 */
/* C99 says that static storage shall be initialized to a null pointer
 * by compiler. */
static unsigned char *frames[MQRSPEC_VERSION_MAX + 1];

/**
 * Put a finder pattern.
 * @param frame
 * @param width
 * @param ox,oy upper-left coordinate of the pattern
 */
static void putFinderPattern(unsigned char *frame, int width, int ox, int oy)
{
	static const unsigned char finder[] = {
		0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1,
		0xc1, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc1,
		0xc1, 0xc0, 0xc1, 0xc1, 0xc1, 0xc0, 0xc1,
		0xc1, 0xc0, 0xc1, 0xc1, 0xc1, 0xc0, 0xc1,
		0xc1, 0xc0, 0xc1, 0xc1, 0xc1, 0xc0, 0xc1,
		0xc1, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc1,
		0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1,
	};
	int x, y;
	const unsigned char *s;

	frame += oy * width + ox;
	s = finder;
	for(y=0; y<7; y++) {
		for(x=0; x<7; x++) {
			frame[x] = s[x];
		}
		frame += width;
		s += 7;
	}
}

static unsigned char *MQRspec_createFrame(int version)
{
	unsigned char *frame, *p, *q;
	int width;
	int x, y;

	width = mqrspecCapacity[version].width;
	frame = (unsigned char *)malloc(width * width);
	memset(frame, 0, width * width);
	/* Finder pattern */
	putFinderPattern(frame, width, 0, 0);
	/* Separator */
	p = frame;
	for(y=0; y<7; y++) {
		p[7] = 0xc0;
		p += width;
	}
	memset(frame + width * 7, 0xc0, 8);
	/* Mask format information area */
	memset(frame + width * 8 + 1, 0x84, 8);
	p = frame + width + 8;
	for(y=0; y<7; y++) {
		*p = 0x84;
		p += width;
	}
	/* Timing pattern */
	p = frame + 8;
	q = frame + width * 8;
	for(x=1; x<width-7; x++) {
		*p =  0x90 | (x & 1);
		*q =  0x90 | (x & 1);
		p++;
		q += width;
	}

	return frame;
}

unsigned char *MQRspec_newFrame(int version)
{
	unsigned char *frame;
	int width;

	if(version < 1 || version > MQRSPEC_VERSION_MAX) return NULL;

	if(frames[version] == NULL) {
		frames[version] = MQRspec_createFrame(version);
	}
	width = mqrspecCapacity[version].width;
	frame = (unsigned char *)malloc(width * width);
	memcpy(frame, frames[version], width * width);

	return frame;
}
