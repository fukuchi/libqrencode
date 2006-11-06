/*
 * qrencode - QR-code encoder
 *
 * Originally written by Y.Swetake
 * Copyright (c)2003-2005 Y.Swetake
 *
 * Ported to C and modified by Kentaro Fukuchi
 * Copyright (c) 2006 Kentaro Fukuchi
 *
 * The following data / specifications are taken from
 * "Two dimensional symbol -- QR-code -- Basic Specification" (JIS X0510:2004)
 *  or
 * "Automatic identification and data capture techniques -- 
 *  QR Code 2005 bar code symbology specification" (ISO/IEC 18004:2006)
 *
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

#include "qrspec.h"

/******************************************************************************
 * Version and capacity
 *****************************************************************************/

QRspec_Capacity qrspecCapacity[QRSPEC_VERSION_MAX + 1] = {
	{  0,    0,    0,    0,    0,    0},
	{ 21,   26,    7,   10,   13,   17}, // 1
	{ 25,   44,   10,   16,   22,   28},
	{ 29,   70,   15,   26,   36,   44},
	{ 33,  100,   20,   36,   52,   64},
	{ 37,  134,   26,   48,   72,   88}, // 5
	{ 41,  172,   36,   64,   96,  112},
	{ 45,  196,   40,   72,  108,  130},
	{ 49,  242,   48,   88,  132,  156},
	{ 53,  292,   60,  110,  160,  192},
	{ 57,  346,   72,  130,  192,  224}, //10
	{ 61,  404,   80,  150,  224,  264},
	{ 65,  466,   96,  176,  260,  308},
	{ 69,  532,  104,  198,  288,  352},
	{ 73,  581,  120,  216,  320,  384},
	{ 77,  655,  132,  240,  360,  432}, //15
	{ 81,  733,  144,  280,  408,  480},
	{ 85,  815,  168,  308,  448,  532},
	{ 89,  901,  180,  338,  504,  588},
	{ 93,  991,  196,  364,  546,  650},
	{ 97, 1085,  224,  416,  600,  700}, //20
	{101, 1156,  224,  442,  644,  750},
	{105, 1258,  252,  476,  690,  816},
	{109, 1364,  280,  504,  750,  900},
	{113, 1474,  300,  560,  810,  960},
	{117, 1588,  312,  588,  870, 1050}, //25
	{121, 1706,  336,  644,  952, 1110},
	{125, 1828,  360,  700, 1020, 1200},
	{129, 1921,  390,  728, 1050, 1260},
	{133, 2051,  420,  784, 1140, 1350},
	{137, 2185,  450,  812, 1200, 1440}, //30
	{141, 2323,  480,  868, 1290, 1530},
	{145, 2465,  510,  924, 1350, 1620},
	{149, 2611,  540,  980, 1440, 1710},
	{153, 2761,  570, 1036, 1530, 1800},
	{157, 2876,  570, 1064, 1590, 1890}, //35
	{161, 3034,  600, 1120, 1680, 1980},
	{165, 3196,  630, 1204, 1770, 2100},
	{169, 3362,  660, 1260, 1860, 2220},
	{173, 3532,  720, 1316, 1950, 2310},
	{177, 3706,  750, 1372, 2040, 2430} //40
};

/**
 * Return a version number that satisfies the input code length.
 * @param size input code length (byte)
 * @return version number
 */
int QRspec_getMinVersion(int size)
{
	int i;

	for(i=1; i<= QRSPEC_VERSION_MAX; i++) {
		if(qrspecCapacity[i].words >= size) return i;
	}

	return -1;
}

/******************************************************************************
 * Length indicator
 *****************************************************************************/

static int lengthTableBits[4][3] = {
	{10, 12, 14},
	{ 9, 11, 13},
	{ 8, 16, 16},
	{ 8, 10, 12}
};

static int lengthTableWords[4][3] = {
	{307, 1228, 4915},
	{ 93,  372, 1489},
	{ 32, 8192, 8192},
	{ 19,   78,  315}
};

int QRspec_lengthIndicator(QRenc_EncodeMode mode, int version)
{
	int l;

	if(version < 9) {
		l = 0;
	} else if(version < 26) {
		l = 1;
	} else {
		l = 2;
	}

	return lengthTableBits[mode][l];
}

int QRspec_maximumWords(QRenc_EncodeMode mode, int version)
{
	int l;

	if(version < 9) {
		l = 0;
	} else if(version < 26) {
		l = 1;
	} else {
		l = 2;
	}

	return lengthTableWords[mode][l];
}
