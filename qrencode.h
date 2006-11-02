/*
 * qrencode - QR-code encoder
 *
 * Originally written by Y.Swetake
 * Copyright (C)2003-2005 Y.Swetake
 *
 * Ported to C and modified by Kentaro Fukuchi
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

#ifndef __QRENCODE_H__
#define __QRENCODE_H__

/*
 * Error Correction Level
 */
/**
 * Level of error correction.
 */
typedef enum {
	QR_EC_LEVEL_L,
	QR_EC_LEVEL_M,
	QR_EC_LEVEL_Q,
	QR_EC_LEVEL_H
} QRenc_ErrorCorrectionLevel;

/**
 * set error correction level.
 */
extern void QRenc_setErrorCorrectionLevel(QRenc_ErrorCorrectionLevel level);
extern QRenc_ErrorCorrectionLevel QRenc_getErrorCorrectionLevel(void);

/**
 * Version
 */
extern int QRenc_getVersion(void);
extern void QRenc_setVersion(int);

/**
 * Encoding mode
 */
typedef enum {
	QR_MODE_NUM,
	QR_MODE_AN,
	QR_MODE_8,
	QR_MODE_KANJI
} QRenc_EncodeMode;

/*
 * Input data stream
 */
/**
 * List structure to store input data stream.
 */
typedef struct _QRenc_DataStream QRenc_DataStream;

extern QRenc_DataStream *QRenc_initData(void);
extern int QRenc_appendData(QRenc_DataStream *stream, QRenc_EncodeMode mode, int size, unsigned char *data);
extern void QRenc_freeData(QRenc_DataStream *stream);

/*
 * QRcode output
 */
extern unsigned char *QRenc_encode(QRenc_DataStream *stream);

#endif /* __QRENCODE_H__ */
