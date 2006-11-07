/*
 * qrencode - QR-code encoder
 *
 * Originally written by Y.Swetake
 * Copyright (c)2003-2005 Y.Swetake
 *
 * Ported to C and modified by Kentaro Fukuchi
 * Copyright (c) 2006 Kentaro Fukuchi
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

/******************************************************************************
 * Entry of input data stream
 *****************************************************************************/

typedef struct _QRenc_List QRenc_List;
struct _QRenc_List {
	QRenc_EncodeMode mode;
	int size;				///< Size of data chunk (byte).
	unsigned char *data;	///< Data chunk.
	BitStream *bstream;
	QRenc_List *next;
};

static QRenc_List *QRenc_newEntry(QRenc_EncodeMode mode, int size, unsigned char *data)
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

struct _QRenc_DataStream {
	int version;
	QRenc_ErrorCorrectionLevel level;
	QRenc_List *head;
	QRenc_List *tail;
};

int QRenc_getVersion(QRenc_DataStream *stream)
{
	return stream->version;
}

void QRenc_setVersion(QRenc_DataStream *stream, int v)
{
	stream->version = v;
}

void QRenc_setErrorCorrectionLevel(QRenc_DataStream *stream, QRenc_ErrorCorrectionLevel level)
{
	stream->level = level;
}

QRenc_ErrorCorrectionLevel QRenc_getErrorCorrectionLevel(QRenc_DataStream *stream)
{
	return stream->level;
}


QRenc_DataStream *QRenc_newData(void)
{
	QRenc_DataStream *stream;

	stream = (QRenc_DataStream *)malloc(sizeof(QRenc_DataStream));
	stream->head = NULL;
	stream->tail = NULL;
	stream->version = 0;
	stream->level = QR_EC_LEVEL_L;

	return stream;
}

int QRenc_appendData(QRenc_DataStream *stream, QRenc_EncodeMode mode, int size, unsigned char *data)
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

void QRenc_freeData(QRenc_DataStream *stream)
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

	val = 0x01;
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

	val = 0x02;
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

	val = 0x04;
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
int QRenc_checkData(QRenc_EncodeMode mode, int size, const unsigned char *data)
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
int QRenc_estimateBitStreamSize(QRenc_DataStream *stream, int version)
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
int QRenc_estimateVersion(QRenc_DataStream *stream)
{
	int bits;
	int new, prev;

	new = 0;
	do {
		prev = new;
		bits = QRenc_estimateBitStreamSize(stream, prev);
		new = QRspec_getMinVersion((bits + 7) / 8);
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
//				QRenc_encodeModeKanji(entry, version);
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
static int QRenc_createBitStream(QRenc_DataStream *stream)
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
static int QRenc_convertData(QRenc_DataStream *stream)
{
}

BitStream *QRenc_mergeBitStream(QRenc_DataStream *stream)
{
	BitStream *bstream;
	QRenc_List *list;

	QRenc_createBitStream(stream);
	bstream = BitStream_new();
	list = stream->head;
	while(list != NULL) {
		BitStream_append(bstream, list->bstream);
		list = list->next;
	}

	return bstream;
}
