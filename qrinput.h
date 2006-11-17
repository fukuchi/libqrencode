/*
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

#ifndef __QRINPUT_H__
#define __QRINPUT_H__

#include "qrencode.h"
#include "bitstream.h"

/******************************************************************************
 * Entry of input data
 *****************************************************************************/
typedef struct _QRenc_List QRenc_List;

/******************************************************************************
 * Input Data
 *****************************************************************************/
struct _QRinput {
	int version;
	QRecLevel level;
	QRenc_List *head;
	QRenc_List *tail;
};

/**
 * Get current error correction level.
 * @param input input data.
 * @return Current error correcntion level.
 */
extern QRecLevel QRenc_getErrorCorrectionLevel(QRinput *input);

/**
 * Set error correction level of the QR-code that is to be encoded.
 * @param input input data.
 * @param level Error correction level.
 */
extern void QRenc_setErrorCorrectionLevel(QRinput *input, QRecLevel level);

/**
 * Get current version.
 * @param input input data.
 * @return current version.
 */
extern int QRenc_getVersion(QRinput *input);

/**
 * Set version of the QR-code that is to be encoded.
 * @param input input data.
 * @param version version number (0 = auto)
 */
extern void QRenc_setVersion(QRinput *input, int version);

/**
 * Pack all bit streams padding bits into a byte array.
 * @param input input data.
 * @return padded merged byte stream
 */
extern unsigned char *QRenc_getByteStream(QRinput *input);

extern int QRenc_estimateBitStreamSize(QRinput *input, int version);
extern BitStream *QRenc_mergeBitStream(QRinput *input);
extern BitStream *QRenc_getBitStream(QRinput *input);

#endif /* __QRINPUT_H__ */
