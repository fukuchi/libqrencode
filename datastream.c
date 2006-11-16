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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "qrencode.h"
#include "qrspec.h"
#include "bitstream.h"
#include "datastream.h"

/******************************************************************************
 * Entry of input data stream
 *****************************************************************************/

struct _QRenc_List {
	QRencodeMode mode;
	int size;				///< Size of data chunk (byte).
	unsigned char *data;	///< Data chunk.
	BitStream *bstream;
	QRenc_List *next;
};

static QRenc_List *QRenc_newEntry(QRencodeMode mode, int size, unsigned char *data)
{
	QRenc_List *entry;

	if(QRenc_checkData(mode, size, data)) {
		return NULL;
	}

	entry = (QRenc_List *)malloc(sizeof(QRenc_List));
	entry->mode = mode;
	entry->size = size;
	entry->data = (unsigned char *)malloc(size);
	memcpy(entry->data, data, size);
	entry->bstream = NULL;
	entry->next = NULL;

	return entry;
}

static QRenc_List *QRenc_freeEntry(QRenc_List *entry)
{
	QRenc_List *next;

	next = entry->next;
	free(entry->data);
	if(entry->bstream) {
		BitStream_free(entry->bstream);
	}
	free(entry);

	return next;
}

/******************************************************************************
 * Input Data stream
 *****************************************************************************/

QRinput *QRenc_newData(void)
{
	QRinput *stream;

	stream = (QRinput *)malloc(sizeof(QRinput));
	stream->head = NULL;
	stream->tail = NULL;
	stream->version = 0;
	stream->level = QR_ECLEVEL_L;

	return stream;
}

int QRenc_getVersion(QRinput *stream)
{
	return stream->version;
}

void QRenc_setVersion(QRinput *stream, int v)
{
	stream->version = v;
}

void QRenc_setErrorCorrectionLevel(QRinput *stream, QRecLevel level)
{
	stream->level = level;
}

QRecLevel QRenc_getErrorCorrectionLevel(QRinput *stream)
{
	return stream->level;
}

int QRenc_appendData(QRinput *stream, QRencodeMode mode, int size, unsigned char *data)
{
	QRenc_List *entry;

	entry = QRenc_newEntry(mode, size, data);
	if(entry == NULL) {
		return -1;
	}

	if(stream->tail == NULL) {
		stream->head = entry;
		stream->tail = entry;
	} else {
		stream->tail->next = entry;
		stream->tail = entry;
	}

	return 0;
}

void QRenc_freeData(QRinput *stream)
{
	QRenc_List *list;

	list = stream->head;
	while(list != NULL) {
		list = QRenc_freeEntry(list);
	}

	free(stream);
}

/******************************************************************************
 * Numeric data
 *****************************************************************************/

/**
 * Check the input data.
 * @param size
 * @param data
 * @return result
 */
static int QRenc_checkModeNum(int size, const char *data)
{
	int i;

	for(i=0; i<size; i++) {
		if(data[i] < '0' || data[i] > '9')
			return -1;
	}

	return 0;
}

/**
 * Estimates the length of the encoded bit stream of numeric data.
 * @param entry
 * @return number of bits
 */
static int QRenc_estimateBitsModeNum(QRenc_List *entry)
{
	int w;
	int bits;

	w = entry->size / 3;
	bits = w * 10;
	switch(entry->size - w * 3) {
		case 1:
			bits += 4;
			break;
		case 2:
			bits += 7;
			break;
		default:
			break;
	}

	return bits;
}

/**
 * Convert the number data stream to a bit stream.
 * @param entry
 */
static void QRenc_encodeModeNum(QRenc_List *entry, int version)
{
	int words;
	int i;
	unsigned int val;

	words = entry->size / 3;
	entry->bstream = BitStream_new();

	val = 0x1;
	BitStream_appendNum(entry->bstream, 4, val);
	
	val = entry->size;
	BitStream_appendNum(entry->bstream, QRspec_lengthIndicator(QR_MODE_NUM, version), val);

	for(i=0; i<words; i++) {
		val  = (entry->data[i*3  ] - '0') * 100;
		val += (entry->data[i*3+1] - '0') * 10;
		val += (entry->data[i*3+2] - '0');

		BitStream_appendNum(entry->bstream, 10, val);
	}

	if(entry->size - words * 3 == 1) {
		val = entry->data[words*3] - '0';
		BitStream_appendNum(entry->bstream, 4, val);
	} else if(entry->size - words * 3 == 2) {
		val  = (entry->data[words*3  ] - '0') * 10;
		val += (entry->data[words*3+1] - '0');
		BitStream_appendNum(entry->bstream, 7, val);
	}
}

/******************************************************************************
 * Alphabet-numeric data
 *****************************************************************************/

static signed char anTable[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	36, -1, -1, -1, 37, 38, -1, -1, -1, -1, 39, 40, -1, 41, 42, 43,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 44, -1, -1, -1, -1, -1,
	-1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
	-1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1
};

/**
 * Look up the alphabet-numeric convesion table (see JIS X0510:2004, pp.19).
 * @param c character
 * @return value
 */
inline static signed char QRenc_lookAnTable(char c)
{
	if(c & 0x80) {
		return -1;
	}
	return anTable[(int)c];
}

/**
 * Check the input data.
 * @param size
 * @param data
 * @return result
 */
static int QRenc_checkModeAn(int size, const char *data)
{
	int i;

	for(i=0; i<size; i++) {
		if(QRenc_lookAnTable(data[i]) < 0)
			return -1;
	}

	return 0;
}

/**
 * Estimates the length of the encoded bit stream of alphabet-numeric data.
 * @param entry
 * @return number of bits
 */
static int QRenc_estimateBitsModeAn(QRenc_List *entry)
{
	int w;
	int bits;

	w = entry->size / 2;
	bits = w * 11;
	if(entry->size & 1) {
		bits += 6;
	}

	return bits;
}

/**
 * Convert the alphabet-numeric data stream to a bit stream.
 * @param entry
 */
static void QRenc_encodeModeAn(QRenc_List *entry, int version)
{
	int words;
	int i;
	unsigned int val;

	words = entry->size / 2;
	entry->bstream = BitStream_new();

	val = 0x2;
	BitStream_appendNum(entry->bstream, 4, val);
	
	val = entry->size;
	BitStream_appendNum(entry->bstream, QRspec_lengthIndicator(QR_MODE_AN, version), val);

	for(i=0; i<words; i++) {
		val  = (unsigned int)QRenc_lookAnTable(entry->data[i*2  ]) * 45;
		val += (unsigned int)QRenc_lookAnTable(entry->data[i*2+1]);

		BitStream_appendNum(entry->bstream, 11, val);
	}

	if(entry->size & 1) {
		val = (unsigned int)QRenc_lookAnTable(entry->data[words * 2]);

		BitStream_appendNum(entry->bstream, 6, val);
	}
}

/******************************************************************************
 * 8 bit data
 *****************************************************************************/

/**
 * Estimates the length of the encoded bit stream of 8 bit data.
 * @param entry
 * @return number of bits
 */
static int QRenc_estimateBitsMode8(QRenc_List *entry)
{
	return entry->size * 8;
}

/**
 * Convert the 8bits data stream to a bit stream.
 * @param entry
 */
static void QRenc_encodeMode8(QRenc_List *entry, int version)
{
	int i;
	unsigned int val;

	entry->bstream = BitStream_new();

	val = 0x4;
	BitStream_appendNum(entry->bstream, 4, val);
	
	val = entry->size;
	BitStream_appendNum(entry->bstream, QRspec_lengthIndicator(QR_MODE_8, version), val);

	for(i=0; i<entry->size; i++) {
		BitStream_appendNum(entry->bstream, 8, entry->data[i]);
	}
}


/******************************************************************************
 * Kanji data
 *****************************************************************************/

/**
 * Estimates the length of the encoded bit stream of kanji data.
 * @param entry
 * @return number of bits
 */
static int QRenc_estimateBitsModeKanji(QRenc_List *entry)
{
	return (entry->size / 2) * 13;
}

/**
 * Check the input data.
 * @param size
 * @param data
 * @return result
 */
static int QRenc_checkModeKanji(int size, const unsigned char *data)
{
	int i;
	unsigned int val;

	if(size & 1)
		return -1;

	for(i=0; i<size; i+=2) {
		val = ((unsigned int)data[i] << 8) | data[i+1];
		if(val < 0x8140 || (val > 0x9ffc && val < 0xe040) || val > 0xebbf) {
			return -1;
		}
	}

	return 0;
}

/**
 * Convert the kanji data stream to a bit stream.
 * @param entry
 */
static void QRenc_encodeModeKanji(QRenc_List *entry, int version)
{
	int i;
	unsigned int val, h;

	entry->bstream = BitStream_new();

	val = 0x8;
	BitStream_appendNum(entry->bstream, 4, val);
	
	val = entry->size / 2;
	BitStream_appendNum(entry->bstream, QRspec_lengthIndicator(QR_MODE_KANJI, version), val);

	for(i=0; i<entry->size; i+=2) {
		val = ((unsigned int)entry->data[i] << 8) | entry->data[i+1];
		if(val <= 0x9ffc) {
			val -= 0x8140;
		} else {
			val -= 0xc140;
		}
		h = (val >> 8) * 0xc0;
		val = (val & 0xff) + h;

		BitStream_appendNum(entry->bstream, 13, val);
	}
}

/******************************************************************************
 * Validation
 *****************************************************************************/

/**
 * Validate the input data
 * @param mode
 * @param size
 * @param data
 * @return result
 */
int QRenc_checkData(QRencodeMode mode, int size, const unsigned char *data)
{
	switch(mode) {
		case QR_MODE_NUM:
			return QRenc_checkModeNum(size, (const char *)data);
			break;
		case QR_MODE_AN:
			return QRenc_checkModeAn(size, (const char *)data);
			break;
		case QR_MODE_KANJI:
			return QRenc_checkModeKanji(size, data);
			break;
		default:
			break;
	}

	return 0;
}

/******************************************************************************
 * Estimation of the bit length
 *****************************************************************************/

/**
 * Estimates the length of the encoded bit stream on the current version.
 * @param entry
 * @param version version of the symbol
 * @return number of bits
 */
static int QRenc_estimateBitStreamSizeOfEntry(QRenc_List *entry, int version)
{
	int bits = 0;
	int l, m;
	int num;

	assert(entry != NULL);
	switch(entry->mode) {
		case QR_MODE_NUM:
			bits = QRenc_estimateBitsModeNum(entry);
			break;
		case QR_MODE_AN:
			bits = QRenc_estimateBitsModeAn(entry);
			break;
		case QR_MODE_8:
			bits = QRenc_estimateBitsMode8(entry);
			break;
		case QR_MODE_KANJI:
			bits = QRenc_estimateBitsModeKanji(entry);
			break;
	}

	l = QRspec_lengthIndicator(entry->mode, version);
	m = 1 << l;
	num = (bits + m - 1) / m;

	bits += num * (4 + l); // mode indicator (4bits) + length indicator

	return bits;
}

/**
 * Estimates the length of the encoded bit stream of the data stream.
 * @param stream data stream
 * @param version version of the symbol
 * @return number of bits
 */
int QRenc_estimateBitStreamSize(QRinput *stream, int version)
{
	QRenc_List *list;
	int bits = 0;

	assert(stream != NULL);

	list = stream->head;
	while(list != NULL) {
		bits += QRenc_estimateBitStreamSizeOfEntry(list, version);
		list = list->next;
	}

	return bits;
}

/**
 * Estimates the required version number of the symbol.
 * @param stream data stream
 * @return required version number
 */
static int QRenc_estimateVersion(QRinput *stream)
{
	int bits;
	int new, prev;

	new = 0;
	do {
		prev = new;
		bits = QRenc_estimateBitStreamSize(stream, prev);
		new = QRspec_getMinimumVersion((bits + 7) / 8, stream->level);
		if (new == -1) {
			return -1;
		}
	} while (new > prev);

	return new;
}

/******************************************************************************
 * Data conversion
 *****************************************************************************/

/**
 * Convert the data stream in the data chunk to a bit stream.
 * @param entry
 * @return number of bits
 */
static int QRenc_encodeBitStream(QRenc_List *entry, int version)
{
	int words;
	QRenc_List *st1, *st2;

	assert(entry != NULL);

	if(entry->bstream != NULL) {
		BitStream_free(entry->bstream);
		entry->bstream = NULL;
	}

	words = QRspec_maximumWords(entry->mode, version);
	if(entry->size > words) {
		st1 = QRenc_newEntry(entry->mode, words, entry->data);
		st2 = QRenc_newEntry(entry->mode, entry->size - words, &entry->data[words]);
		QRenc_encodeBitStream(st1, version);
		QRenc_encodeBitStream(st2, version);
		entry->bstream = BitStream_new();
		BitStream_append(entry->bstream, st1->bstream);
		BitStream_append(entry->bstream, st2->bstream);
		QRenc_freeEntry(st1);
		QRenc_freeEntry(st2);
	} else {
		switch(entry->mode) {
			case QR_MODE_NUM:
				QRenc_encodeModeNum(entry, version);
				break;
			case QR_MODE_AN:
				QRenc_encodeModeAn(entry, version);
				break;
			case QR_MODE_8:
				QRenc_encodeMode8(entry, version);
				break;
			case QR_MODE_KANJI:
				QRenc_encodeModeKanji(entry, version);
				break;
			default:
				break;
		}
	}

	return BitStream_size(entry->bstream);
}

/**
 * Convert the input data stream to a bit stream.
 * @param stream input data stream.
 * @return length of the bit stream.
 */
static int QRenc_createBitStream(QRinput *stream)
{
	QRenc_List *list;
	int bits = 0;

	assert(stream != NULL);

	list = stream->head;
	while(list != NULL) {
		bits += QRenc_encodeBitStream(list, stream->version);
		list = list->next;
	}

	return bits;
}

/**
 * Convert the input data stream to a bit stream.
 * When the version number is given and that is not sufficient, it is increased
 * automatically.
 * @param stream input data stream.
 * @return -1 if the input data was too large. Otherwise 0.
 */
static int QRenc_convertData(QRinput *stream)
{
	int bits;
	int ver;

	ver = QRenc_estimateVersion(stream);
	if(ver > QRenc_getVersion(stream)) {
		QRenc_setVersion(stream, ver);
	}

	for(;;) {
		bits = QRenc_createBitStream(stream);
		ver = QRspec_getMinimumVersion((bits + 7) / 8, stream->level);
		if(ver < 0) {
			return -1;
		} else if(ver > QRenc_getVersion(stream)) {
			QRenc_setVersion(stream, ver);
		} else {
			break;
		}
	}

	return 0;
}

/**
 * Create padding bits for the input stream.
 * @param stream input data stream.
 * @return padding bit stream.
 */
static BitStream *QRenc_createPaddingBit(QRinput *stream)
{
	int bits, maxbits, words, maxwords, i;
	QRenc_List *list;
	BitStream *bstream;

	if(stream->version <= 0)
		return NULL;

	maxwords = QRspec_getDataLength(stream->version, stream->level);
	maxbits = maxwords * 8;
	
	list = stream->head;
	bits = 0;
	while(list != NULL) {
		bits += BitStream_size(list->bstream);
		list = list->next;
	}

	words = (bits + 7) / 8;

	if(bits == maxbits)
		return NULL;

	if(maxbits - bits < 5) {
		bstream = BitStream_new();
		BitStream_appendNum(bstream, maxbits - bits, 0);
		return bstream;
	}

	bstream = BitStream_new();
	BitStream_appendNum(bstream, words * 8 - bits, 0);

	for(i=0; i<maxwords - words; i++) {
		if(i & 1) {
			BitStream_appendNum(bstream, 8, 0x11);
		} else {
			BitStream_appendNum(bstream, 8, 0xec);
		}
	}

	return bstream;
}

/**
 * Merge all bit streams in the input data stream
 * @param stream input data stream.
 * @return merged bit stream
 */

BitStream *QRenc_mergeBitStream(QRinput *stream)
{
	BitStream *bstream;
	QRenc_List *list;

	if(QRenc_convertData(stream) < 0) {
		return NULL;
	}

	bstream = BitStream_new();
	list = stream->head;
	while(list != NULL) {
		BitStream_append(bstream, list->bstream);
		list = list->next;
	}

	return bstream;
}

/**
 * Merge all bit streams in the input data stream and append padding bits
 * @param stream input data stream.
 * @return padded merged bit stream
 */

BitStream *QRenc_getBitStream(QRinput *stream)
{
	BitStream *bstream;
	BitStream *padding;

	bstream = QRenc_mergeBitStream(stream);
	if(bstream == NULL) {
		return NULL;
	}
	padding = QRenc_createPaddingBit(stream);
	BitStream_append(bstream, padding);
	BitStream_free(padding);

	return bstream;
}

/**
 * Pack all bit streams padding bits into a byte array.
 * @param stream input data stream.
 * @return padded merged byte stream
 */

unsigned char *QRenc_getByteStream(QRinput *stream)
{
	BitStream *bstream;
	unsigned char *array;

	bstream = QRenc_getBitStream(stream);
	array = BitStream_toByte(bstream);
	BitStream_free(bstream);

	return array;
}
