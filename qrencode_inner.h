/**
 * qrencode - QR Code encoder
 *
 * Header for internal use
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
	int version;
	unsigned char *datacode;
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
 * Format information
 *****************************************************************************/
extern void QRcode_writeFormatInformation(int width, unsigned char *frame, int mask, QRecLevel level);

extern QRcode *QRcode_encodeMask(QRinput *input, int version, QRecLevel level, int mask);
#endif /* __QRENCODE_INNER_H__ */
