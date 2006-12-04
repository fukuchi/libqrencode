/**
 * qrencode - QR Code encoder
 *
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

/** \mainpage
 * Libqrencode is a library for encoding data in a QR Code symbol, a kind of 2D
 * symbology.
 *
 * <h2>Encoding</h2>
 * 
 * There are two ways to encode data: <b>encoding a string</b> or 
 * <b>encoding a structured data</b>.
 *
 * <h3>Encoding a string</h3>
 * You can encode a string by calling QRcode_encodeString().
 * The given string is parsed automatically and encoded. If you want to encode
 * data that can be represented as a C string style (NUL terminated), you can
 * simply use this way.
 *
 * If the input data contains Kanji (Shift-JIS) characters and you want to
 * encode them as Kanji in QR Code, you should give QR_MODE_KANJI as a hint.
 * Otherwise, all of non-alphanumeric characters are encoded as 8 bit data.
 *
 * Please note that a C string can not contain NUL character. If your data
 * contains NUL, you should chose the second way.
 *
 * <h3>Encoding a structured data</h3>
 * You can construct a structured input data manually. If the structure of the
 * input data is known, you can use this way.
 * At first, you must create a ::QRinput object by QRinput_new(). Then, you can
 * add input data to the QRinput object by QRinput_append().
 * Finally you can call QRcode_encodeInput() to encode the QRinput data.
 * You can reuse the QRinput data again to encode it in other symbols with
 * different parameters.
 *
 * <h2>Result</h2>
 * The encoded symbol is resulted as a ::QRcode object. It will contain
 * its version number, width of the symbol and an array represents the symbol.
 * See ::QRcode for the details. You can free the object by QRcode_free().
 *
 * Please note that the version of the result may be larger than specified.
 * In such cases, the input data would be too large to be encoded in the
 * symbol of the specified version.
 *
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
 * meaningless for usual applications, but here the specification is described.
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
 *             encoded. If QR_MODE_KANJI is given, kanji characters will be
 *             encoded as Shif-JIS characters. If QR_MODE_8 is given, all of
 *             non-alphanumerical characters will be encoded as is. If you want
 *             to embed UTF-8 string, choose this.
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
