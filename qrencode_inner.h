/**
 * qrencode - QR Code encoder
 *
 * Header for test use
 * Copyright (C) 2006, 2007, 2008, 2009 Kentaro Fukuchi <fukuchi@megaui.net>
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

#ifndef __QRENCODE_INNER_H__
#define __QRENCODE_INNER_H__

/**
 * This header file includes definitions for test use.
 */
extern BitStream *QRinput_mergeBitStream(QRinput *input);
extern BitStream *QRinput_getBitStream(QRinput *input);
extern int QRinput_estimateBitStreamSize(QRinput *input, int version);
extern int QRinput_splitEntry(QRinput_List *entry, int bytes);
extern int QRinput_lengthOfCode(QRencodeMode mode, int version, int bits);
extern int QRinput_insertStructuredAppendHeader(QRinput *input, int size, int index, unsigned char parity);


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
	int version;
	unsigned char *datacode;
	unsigned char *ecccode;
	int blocks;
	RSblock *rsblock;
	int count;
	int dataLength;
	int eccLength;
	int b1;
} QRRawCode;

extern QRRawCode *QRraw_new(QRinput *input);
extern unsigned char QRraw_getCode(QRRawCode *raw);
extern void QRraw_free(QRRawCode *raw);

/******************************************************************************
 * Raw code for Micro QR Code
 *****************************************************************************/

typedef struct {
	int version;
	unsigned char *datacode;
	RSblock *rsblock;
	int count;
	int dataLength;
	int eccLength;
} MQRRawCode;

extern MQRRawCode *MQRraw_new(QRinput *input);
extern unsigned char MQRraw_getCode(MQRRawCode *raw);
extern void MQRraw_free(MQRRawCode *raw);

/******************************************************************************
 * Frame filling
 *****************************************************************************/
extern unsigned char *FrameFiller_fillerTest(int version);

/******************************************************************************
 * QR-code encoding
 *****************************************************************************/
extern QRcode *QRcode_encodeMask(QRinput *input, int mask);


/******************************************************************************
 * Mask
 *****************************************************************************/

extern int Mask_evaluateSymbol(int width, unsigned char *frame);
extern int Mask_writeFormatInformation(int width, unsigned char *frame, int mask, QRecLevel level);

#endif /* __QRENCODE_INNER_H__ */
