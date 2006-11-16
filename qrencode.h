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

#ifndef __QRENCODE_H__
#define __QRENCODE_H__

/**
 * Encoding mode.
 */
typedef enum {
	QR_MODE_NUM = 0,	///< Numeric mode
	QR_MODE_AN,			///< Alphabet-numeric mode
	QR_MODE_8,			///< 8-bit data mode
	QR_MODE_KANJI		///< Kanji (shift-jis) mode
} QRencodeMode;

/**
 * Level of error correction.
 */
typedef enum {
	QR_ECLEVEL_L = 0,
	QR_ECLEVEL_M,
	QR_ECLEVEL_Q,
	QR_ECLEVEL_H
} QRecLevel;

/******************************************************************************
 * Input data stream
 *****************************************************************************/

/**
 * Data structure to store input data stream.
 */
typedef struct _QRinput QRinput;

/**
 * Instantiate a data stream object.
 * @return Stream object (initialized).
 */
extern QRinput *QRenc_newData(void);

/**
 * Get current error correction level.
 * @param stream input data stream
 * @return Current error correcntion level.
 */
extern QRecLevel QRenc_getErrorCorrectionLevel(QRinput *stream);

/**
 * Set error correction level of the QR-code that is to be encoded.
 * @param stream input data stream
 * @param level Error correction level.
 */
extern void QRenc_setErrorCorrectionLevel(QRinput *stream, QRecLevel level);

/**
 * Get current version.
 * @param stream input data stream
 * @return current version
 */
extern int QRenc_getVersion(QRinput *stream);

/**
 * Set version of the QR-code that is to be encoded.
 * @param stream input data stream
 * @param version version number (0 = auto)
 */
extern void QRenc_setVersion(QRinput *stream, int version);

/**
 * Append data to the stream object.
 * The data is copied and appended to the stream object.
 * @param stream Stream object.
 * @param mode Encoding mode.
 * @param size Size of data (byte).
 * @param data A pointer to the memory area of the input data.
 * @return -1 when the input data is invalid. Otherwise 0.
 */
extern int QRenc_appendData(QRinput *stream, QRencodeMode mode, int size, unsigned char *data);

/**
 * Free the stream object.
 * All of data chunks in the stream object are freed too.
 * @param stream Stream object.
 */
extern void QRenc_freeData(QRinput *stream);

/**
 * Validate the input data
 * @param mode
 * @param size
 * @param data
 * @return result
 */
extern int QRenc_checkData(QRencodeMode mode, int size, const unsigned char *data);

/******************************************************************************
 * QRcode output
 *****************************************************************************/

/**
 * QRcode class.
 */
typedef struct {
	int version; //< version of the symbol
	int width; //< width of the symbol
	unsigned char *data; //< symbol data
} QRcode;

/**
 * Create a symbol from the input data stream.
 * @param stream input data stream.
 * @return an instance of QRcode class.
 */
extern QRcode *QRenc_encode(QRinput *stream);

/**
 * Free the instance of QRcode class.
 * @param qrcode an instance of QRcode class.
 */
extern void QRenc_freeQRcode(QRcode *qrcode);

#endif /* __QRENCODE_H__ */
