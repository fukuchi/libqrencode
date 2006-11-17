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

#ifndef __QRENCODE_INNER_H__
#define __QRENCODE_INNER_H__

/**
 * This header file includes definitions for inner use.
 */

/******************************************************************************
 * Raw code
 *****************************************************************************/
typedef struct {
	int dataLength;
	unsigned char *data;
	int eccLength;
	unsigned char *ecc;
} RSblock;

typedef struct {
	unsigned char *datacode;
	int blocks;
	RSblock *rsblock;
	int count;
	int dataLength;
	int eccLength;
	int b1;
	int b2;
} QRRawCode;

extern QRRawCode *QRraw_new(QRinput *stream);
extern unsigned char QRraw_getCode(QRRawCode *raw);
extern void QRraw_free(QRRawCode *raw);

/******************************************************************************
 * Frame filling
 *****************************************************************************/
extern unsigned char *QRenc_fillerTest(int version);

/******************************************************************************
 * Format information
 *****************************************************************************/
extern void QRenc_writeFormatInformation(int width, unsigned char *frame, int mask, QRecLevel level);

/******************************************************************************
 * Masking
 *****************************************************************************/
extern unsigned char *QRenc_makeMask(int width, unsigned char *frame, int mask);

extern int QRenc_evaluateSymbol(int width, unsigned char *frame);

QRcode *QRcode_encodeMask(QRinput *stream, int version, QRecLevel level, int mask);

#endif /* __QRENCODE_INNER_H__ */
