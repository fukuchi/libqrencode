/**
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
 *
 */

#ifndef __QRENCODE_H__
#define __QRENCODE_H__

/******************************************************************************
 * Error Correction Level
 *****************************************************************************/

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
 * Get current error correction level.
 * @return Current error correcntion level.
 */
extern QRenc_ErrorCorrectionLevel QRenc_getErrorCorrectionLevel(void);

/**
 * Set error correction level of the QR-code that is to be encoded.
 * @param level Error correction level.
 */
extern void QRenc_setErrorCorrectionLevel(QRenc_ErrorCorrectionLevel level);

/******************************************************************************
 * Version (size of the QR-code)
 *****************************************************************************/

/**
 * Get current version.
 * @return Current version.
 */
extern int QRenc_getVersion(void);

/**
 * Set version of the QR-code that is to be encoded.
 * @param version Version. (0 = auto)
 */
extern void QRenc_setVersion(int version);

/******************************************************************************
 * Encoding mode
 *****************************************************************************/

/**
 * Encoding mode.
 */
typedef enum {
	QR_MODE_NUM = 0,	///< Numeric mode
	QR_MODE_AN,			///< Alphabet-numeric mode
	QR_MODE_8,			///< 8-bit data mode
	QR_MODE_KANJI		///< Kanji (shift-jis) mode
} QRenc_EncodeMode;

/******************************************************************************
 * Input data stream
 *****************************************************************************/

/**
 * Data structure to store input data stream.
 */
typedef struct _QRenc_DataStream QRenc_DataStream;

/**
 * Instantiate a data stream object.
 * @return Stream object (initialized).
 */
extern QRenc_DataStream *QRenc_newData(void);

/**
 * Append data to the stream object.
 * The data is copied and appended to the stream object.
 * @param stream Stream object.
 * @param mode Encoding mode.
 * @param size Size of data (byte).
 * @param data A pointer to the memory area of the input data.
 * @return Always 0.
 */
extern int QRenc_appendData(QRenc_DataStream *stream, QRenc_EncodeMode mode, int size, unsigned char *data);

/**
 * Free the stream object.
 * All of data chunks in the stream object are freed too.
 * @param stream Stream object.
 */
extern void QRenc_freeData(QRenc_DataStream *stream);

/******************************************************************************
 * QRcode output
 *****************************************************************************/
extern unsigned char *QRenc_encode(QRenc_DataStream *stream);

#endif /* __QRENCODE_H__ */
