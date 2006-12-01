/**
 * qrencode - QR Code encoder
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
	QR_ECLEVEL_L = 0, ///< lowest
	QR_ECLEVEL_M,
	QR_ECLEVEL_Q,
	QR_ECLEVEL_H      ///< highest
} QRecLevel;

/******************************************************************************
 * Input data
 *****************************************************************************/

/**
 * Data structure to store input data.
 */
typedef struct _QRinput QRinput;

/**
 * Instantiate an input data object.
 * @return input object (initialized).
 */
extern QRinput *QRinput_new(void);

/**
 * Append data to the input object.
 * The data is copied and appended to the input object.
 * @param input input object.
 * @param mode encoding mode.
 * @param size size of data (byte).
 * @param data a pointer to the memory area of the input data.
 * @return -1 when the input data is invalid. Otherwise, return 0.
 */
extern int QRinput_append(QRinput *input, QRencodeMode mode, int size, const unsigned char *data);

/**
 * Free the input object.
 * All of data chunks in the input object are freed too.
 * @param input input object.
 */
extern void QRinput_free(QRinput *input);

/**
 * Validate the input data.
 * @param mode
 * @param size
 * @param data
 * @return result return -1 if the input is invalid. Otherwise, return 0.
 */
extern int QRinput_check(QRencodeMode mode, int size, const unsigned char *data);

/******************************************************************************
 * QRcode output
 *****************************************************************************/

/**
 * QRcode class.
 * Symbol data is represented as an array contains width*width uchars.
 * Each uchar represents a module (dot). If the less significant bit of
 * the uchar is 1, the corresponding module is black. The other bits are
 * meaningless for usual application, but here the specification is described.
 *
 * <pre>
 * MSB 76543210 LSB
 *     |||||||`- 1=black/0=white
 *     ||||||`-- data and ecc code area
 *     |||||`--- format information
 *     ||||`---- version information
 *     |||`----- timing pattern
 *     ||`------ alignment pattern
 *     |`------- finder pattern and separator
 *     `-------- non-data modules (format, timing, etc.)
 * </pre>
 */
typedef struct {
	int version;	///< version of the symbol
	int width;		///< width of the symbol
	unsigned char *data; ///< symbol data
} QRcode;

/**
 * Create a symbol from the input data.
 * @param input input data.
 * @param version version of the symbol. If 0, the library chooses the minimum
 *                version for the input data.
 * @param level error correction level.
 * @return an instance of QRcode class. The version of the result QRcode may
 *         be larger than the designated version.
 */
extern QRcode *QRcode_encodeInput(QRinput *input, int version, QRecLevel level);

/**
 * Create a symbol from the string. The library automatically parses the input
 * string and encodes in a QR Code symbol.
 * @param string input string. It should be NULL terminated.
 * @param version version of the symbol. If 0, the library chooses the minimum
 *                version for the input data.
 * @param level error correction level.
 * @param hint tell the library how non-alphanumerical characters should be
 *             encoded. If QR_MODE_KANJI is given, those characters will be
 *             encoded as Shif-JIS characters. If QR_MODE_8 is given, they wil
 *             be encoded as is. If you want to embed UTF-8 string, choose this.
 * @return an instance of QRcode class. The version of the result QRcode may
 *         be larger than the designated version.
 */
extern QRcode *QRcode_encodeString(const char *string, int version, QRecLevel level, QRencodeMode hint);

/**
 * Free the instance of QRcode class.
 * @param qrcode an instance of QRcode class.
 */
extern void QRcode_free(QRcode *qrcode);

#endif /* __QRENCODE_H__ */
