/**
 * qrencode - QR-code encoder
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
 * Level of error correction.
 */
typedef enum {
	QR_EC_LEVEL_L = 0,
	QR_EC_LEVEL_M,
	QR_EC_LEVEL_Q,
	QR_EC_LEVEL_H
} QRenc_ErrorCorrectionLevel;

/**
 * Get current error correction level.
 * @param stream input data stream
 * @return Current error correcntion level.
 */
extern QRenc_ErrorCorrectionLevel QRenc_getErrorCorrectionLevel(QRenc_DataStream *stream);

/**
 * Set error correction level of the QR-code that is to be encoded.
 * @param stream input data stream
 * @param level Error correction level.
 */
extern void QRenc_setErrorCorrectionLevel(QRenc_DataStream *stream, QRenc_ErrorCorrectionLevel level);

/**
 * Get current version.
 * @param stream input data stream
 * @return current version
 */
extern int QRenc_getVersion(QRenc_DataStream *stream);

/**
 * Set version of the QR-code that is to be encoded.
 * @param stream input data stream
 * @param version version number (0 = auto)
 */
extern void QRenc_setVersion(QRenc_DataStream *stream, int version);


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
 * @return -1 when the input data is invalid. Otherwise 0.
 */
extern int QRenc_appendData(QRenc_DataStream *stream, QRenc_EncodeMode mode, int size, unsigned char *data);

/**
 * Free the stream object.
 * All of data chunks in the stream object are freed too.
 * @param stream Stream object.
 */
extern void QRenc_freeData(QRenc_DataStream *stream);

/**
 * Validate the input data
 * @param mode
 * @param size
 * @param data
 * @return result
 */
extern int QRenc_checkData(QRenc_EncodeMode mode, int size, const unsigned char *data);

/******************************************************************************
 * QRcode output
 *****************************************************************************/
extern unsigned char *QRenc_encode(QRenc_DataStream *stream);

#endif /* __QRENCODE_H__ */
