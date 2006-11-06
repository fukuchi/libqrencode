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
 * Error Correction Level
 *****************************************************************************/

static QRenc_ErrorCorrectionLevel errorCorrectionLevel = QR_EC_LEVEL_L;

void QRenc_setErrorCorrectionLevel(QRenc_ErrorCorrectionLevel level)
{
	errorCorrectionLevel = level;
}

QRenc_ErrorCorrectionLevel QRenc_getErrorCorrectionLevel(void)
{
	return errorCorrectionLevel;
}

/******************************************************************************
 * Version (size of the QR-code)
 *****************************************************************************/

static int version = 0;

extern int QRenc_getVersion(void)
{
	return version;
}

extern void QRenc_setVersion(int v)
{
	version = v;
}

/******************************************************************************
 * Input data stream
 *****************************************************************************/

typedef struct _QRenc_List QRenc_List;
struct _QRenc_List {
	QRenc_EncodeMode mode;
	int size;				///< Size of data chunk (byte).
	unsigned char *data;	///< Data chunk.
	BitStream *bstream;
	QRenc_List *next;
};

struct _QRenc_DataStream {
	QRenc_List *head;
	QRenc_List *tail;
};


QRenc_DataStream *QRenc_newData(void)
{
	QRenc_DataStream *stream;

	stream = (QRenc_DataStream *)malloc(sizeof(QRenc_DataStream));
	stream->head = NULL;
	stream->tail = NULL;

	return stream;
}

int QRenc_appendData(QRenc_DataStream *stream, QRenc_EncodeMode mode, int size, unsigned char *data)
{
	QRenc_List *entry;

	entry = (QRenc_List *)malloc(sizeof(QRenc_List));
	entry->mode = mode;
	entry->size = size;
	entry->data = (unsigned char *)malloc(size);
	entry->bstream = NULL;

	memcpy(entry->data, data, size);
	entry->next = NULL;

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
	QRenc_List *list, *next;

	list = stream->head;
	while(list != NULL) {
		free(list->data);
		next = list->next;
		if(list->bstream != NULL) {
			BitStream_free(list->bstream);
		}
		free(list);
		list = next;
	}

	free(stream);
}

/******************************************************************************
 * Data conversion
 *****************************************************************************/


/**
 * Convert the number data stream to a bit stream.
 * @param entry
 * @return number of bits
 */
static int QRenc_encodeModeNum(QRenc_List *entry)
{
	int bits;
	int words;
	int i;
	unsigned int val;
	
	words = entry->size / 3;
	bits = words * 8;
	switch(entry->size - words * 3) {
		case 1:
			bits += 4;
			break;
		case 2:
			bits += 7;
			break;
		default:
			break;
	}

	entry->bstream = BitStream_new(bits);

	for(i=0; i<words; i++) {
		val  = (entry->data[i*3  ] - '0') * 100;
		val += (entry->data[i*3+1] - '0') * 10;
		val += (entry->data[i*3+2] - '0');
	}
}

/**
 * Convert the data stream in the data chunk to a bit stream.
 * @param entry
 * @return number of bits
 */
static int QRenc_encodeBitStream(QRenc_List *entry)
{
	assert(entry != NULL);
	switch(entry->mode) {
		case QR_MODE_NUM:
			return QRenc_encodeModeNum(entry);
			break;
		case QR_MODE_AN:
//			return QRenc_encodeModeAn(entry);
			break;
		case QR_MODE_8:
//			return QRenc_encodeMode8(entry);
			break;
		case QR_MODE_KANJI:
//			return QRenc_encodeModeKanji(entry);
			break;
	}
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
 * Estimates the length of the encoded bit stream of 8 bit data.
 * @param entry
 * @return number of bits
 */
static int QRenc_estimateBitsMode8(QRenc_List *entry)
{
	return entry->size * 8;
}

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
 * Convert the input data stream to a bit stream.
 * @param stream input data stream.
 * @return bit stream.
 */
int QRenc_createBitStream(QRenc_DataStream *stream)
{
	QRenc_List *list;
	int bits = 0;
	BitStream *bstream;

	assert(stream != NULL);

	list = stream->head;
	while(list != NULL) {
		bits += QRenc_encodeBitStream(list);
		list = list->next;
	}

	return bits;
}
