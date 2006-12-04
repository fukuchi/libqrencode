/*
 * qrencode - QR Code encoder
 *
 * Input data chunk class
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

#ifndef __QRINPUT_H__
#define __QRINPUT_H__

#include "qrencode.h"
#include "bitstream.h"

/******************************************************************************
 * Entry of input data
 *****************************************************************************/
typedef struct _QRinput_List QRinput_List;

struct _QRinput_List {
	QRencodeMode mode;
	int size;				///< Size of data chunk (byte).
	unsigned char *data;	///< Data chunk.
	BitStream *bstream;
	QRinput_List *next;
};

/******************************************************************************
 * Input Data
 *****************************************************************************/
struct _QRinput {
	int version;
	QRecLevel level;
	QRinput_List *head;
	QRinput_List *tail;
};

/**
 * Get current error correction level.
 * @param input input data.
 * @return Current error correcntion level.
 */
extern QRecLevel QRinput_getErrorCorrectionLevel(QRinput *input);

/**
 * Set error correction level of the QR-code that is to be encoded.
 * @param input input data.
 * @param level Error correction level.
 */
extern void QRinput_setErrorCorrectionLevel(QRinput *input, QRecLevel level);

/**
 * Get current version.
 * @param input input data.
 * @return current version.
 */
extern int QRinput_getVersion(QRinput *input);

/**
 * Set version of the QR-code that is to be encoded.
 * @param input input data.
 * @param version version number (0 = auto)
 */
extern void QRinput_setVersion(QRinput *input, int version);

/**
 * Pack all bit streams padding bits into a byte array.
 * @param input input data.
 * @return padded merged byte stream
 */
extern unsigned char *QRinput_getByteStream(QRinput *input);

extern int QRinput_estimateBitsModeNum(int size);
extern int QRinput_estimateBitsModeAn(int size);
extern int QRinput_estimateBitsMode8(int size);
extern int QRinput_estimateBitsModeKanji(int size);

extern int QRinput_estimateBitStreamSize(QRinput *input, int version);
extern BitStream *QRinput_mergeBitStream(QRinput *input);
extern BitStream *QRinput_getBitStream(QRinput *input);

extern const signed char QRinput_anTable[];
extern QRencodeMode QRinput_identifyMode(const char *string);

/**
 * Look up the alphabet-numeric convesion table (see JIS X0510:2004, pp.19).
 * @param __c__ character
 * @return value
 */
#define QRinput_lookAnTable(__c__) \
	((__c__ & 0x80)?-1:QRinput_anTable[(int)__c__])

#endif /* __QRINPUT_H__ */
