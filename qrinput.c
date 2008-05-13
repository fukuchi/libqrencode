/*
 * qrencode - QR Code encoder
 *
 * Input data chunk class
 * Copyright (C) 2006, 2007, 2008 Kentaro Fukuchi <fukuchi@megaui.net>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "qrencode.h"
#include "qrspec.h"
#include "bitstream.h"
#include "qrinput.h"

/******************************************************************************
 * Entry of input data
 *****************************************************************************/

static QRinput_List *QRinput_List_newEntry(QRencodeMode mode, int size, const unsigned char *data)
{
	QRinput_List *entry;

	if(QRinput_check(mode, size, data)) {
		errno = EINVAL;
		return NULL;
	}

	entry = (QRinput_List *)malloc(sizeof(QRinput_List));
	if(entry == NULL) return NULL;

	entry->mode = mode;
	entry->size = size;
	entry->data = (unsigned char *)malloc(size);
	if(entry->data == NULL) {
		free(entry);
		return NULL;
	}
	memcpy(entry->data, data, size);
	entry->bstream = NULL;
	entry->next = NULL;

	return entry;
}

static QRinput_List *QRinput_List_freeEntry(QRinput_List *entry)
{
	QRinput_List *next;

	next = entry->next;
	free(entry->data);
	if(entry->bstream) {
		BitStream_free(entry->bstream);
	}
	free(entry);

	return next;
}

static QRinput_List *QRinput_List_dup(QRinput_List *entry)
{
	QRinput_List *n;

	n = (QRinput_List *)malloc(sizeof(QRinput_List));
	if(n == NULL) return NULL;

	n->mode = entry->mode;
	n->size = entry->size;
	n->data = (unsigned char *)malloc(n->size);
	if(n->data == NULL) {
		free(n);
		return NULL;
	}
	memcpy(n->data, entry->data, entry->size);
	n->bstream = NULL;
	n->next = NULL;

	return n;
}

/******************************************************************************
 * Input Data
 *****************************************************************************/

QRinput *QRinput_new(void)
{
	return QRinput_new2(0, QR_ECLEVEL_L);
}

QRinput *QRinput_new2(int version, QRecLevel level)
{
	QRinput *input;

	if(version < 0 || version > QRSPEC_VERSION_MAX || level < QR_ECLEVEL_L || level > QR_ECLEVEL_H) {
		errno = EINVAL;
		return NULL;
	}

	input = (QRinput *)malloc(sizeof(QRinput));
	if(input == NULL) return NULL;

	input->head = NULL;
	input->tail = NULL;
	input->version = version;
	input->level = level;

	return input;
}

int QRinput_getVersion(QRinput *input)
{
	return input->version;
}

int QRinput_setVersion(QRinput *input, int version)
{
	if(version < 0 || version > QRSPEC_VERSION_MAX) {
		errno = EINVAL;
		return -1;
	}

	input->version = version;

	return 0;
}

QRecLevel QRinput_getErrorCorrectionLevel(QRinput *input)
{
	return input->level;
}

int QRinput_setErrorCorrectionLevel(QRinput *input, QRecLevel level)
{
	if(level < QR_ECLEVEL_L || level > QR_ECLEVEL_H) {
		errno = EINVAL;
		return -1;
	}

	input->level = level;

	return 0;
}

static void QRinput_appendEntry(QRinput *input, QRinput_List *entry)
{
	if(input->tail == NULL) {
		input->head = entry;
		input->tail = entry;
	} else {
		input->tail->next = entry;
		input->tail = entry;
	}
	entry->next = NULL;
}

int QRinput_append(QRinput *input, QRencodeMode mode, int size, const unsigned char *data)
{
	QRinput_List *entry;

	entry = QRinput_List_newEntry(mode, size, data);
	if(entry == NULL) {
		return -1;
	}

	QRinput_appendEntry(input, entry);

	return 0;
}

int QRinput_insertStructuredAppendHeader(QRinput *input, int size, int index, unsigned char parity)
{
	QRinput_List *entry;
	unsigned char buf[3];

	if(size > MAX_STRUCTURED_SYMBOLS) {
		errno = EINVAL;
		return -1;
	}
	if(index <= 0 || index > MAX_STRUCTURED_SYMBOLS) {
		errno = EINVAL;
		return -1;
	}

	buf[0] = (unsigned char)size;
	buf[1] = (unsigned char)index;
	buf[2] = parity;
	entry = QRinput_List_newEntry(QR_MODE_STRUCTURE, 3, buf);
	if(entry == NULL) {
		return -1;
	}

	entry->next = input->head;
	input->head = entry;

	return 0;
}

void QRinput_free(QRinput *input)
{
	QRinput_List *list;

	list = input->head;
	while(list != NULL) {
		list = QRinput_List_freeEntry(list);
	}

	free(input);
}

static unsigned char QRinput_calcParity(QRinput *input)
{
	unsigned char parity = 0;
	QRinput_List *list;
	int i;

	list = input->head;
	while(list != NULL) {
		if(list->mode != QR_MODE_STRUCTURE) {
			for(i=list->size-1; i>=0; i--) {
				parity ^= list->data[i];
			}
		}
		list = list->next;
	}

	return parity;
}

QRinput *QRinput_dup(QRinput *input)
{
	QRinput *n;
	QRinput_List *list, *e;

	n = QRinput_new2(input->version, input->level);
	if(n == NULL) return NULL;

	list = input->head;
	while(list != NULL) {
		e = QRinput_List_dup(list);
		QRinput_appendEntry(n, e);
		list = list->next;
	}

	return n;
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
static int QRinput_checkModeNum(int size, const char *data)
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
 * @param size
 * @return number of bits
 */
int QRinput_estimateBitsModeNum(int size)
{
	int w;
	int bits;

	w = size / 3;
	bits = w * 10;
	switch(size - w * 3) {
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
 * Convert the number data to a bit stream.
 * @param entry
 */
static void QRinput_encodeModeNum(QRinput_List *entry, int version)
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

const signed char QRinput_anTable[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	36, -1, -1, -1, 37, 38, -1, -1, -1, -1, 39, 40, -1, 41, 42, 43,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 44, -1, -1, -1, -1, -1,
	-1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

/**
 * Check the input data.
 * @param size
 * @param data
 * @return result
 */
static int QRinput_checkModeAn(int size, const char *data)
{
	int i;

	for(i=0; i<size; i++) {
		if(QRinput_lookAnTable(data[i]) < 0)
			return -1;
	}

	return 0;
}

/**
 * Estimates the length of the encoded bit stream of alphabet-numeric data.
 * @param size
 * @return number of bits
 */
int QRinput_estimateBitsModeAn(int size)
{
	int w;
	int bits;

	w = size / 2;
	bits = w * 11;
	if(size & 1) {
		bits += 6;
	}

	return bits;
}

/**
 * Convert the alphabet-numeric data to a bit stream.
 * @param entry
 */
static void QRinput_encodeModeAn(QRinput_List *entry, int version)
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
		val  = (unsigned int)QRinput_lookAnTable(entry->data[i*2  ]) * 45;
		val += (unsigned int)QRinput_lookAnTable(entry->data[i*2+1]);

		BitStream_appendNum(entry->bstream, 11, val);
	}

	if(entry->size & 1) {
		val = (unsigned int)QRinput_lookAnTable(entry->data[words * 2]);

		BitStream_appendNum(entry->bstream, 6, val);
	}
}

/******************************************************************************
 * 8 bit data
 *****************************************************************************/

/**
 * Estimates the length of the encoded bit stream of 8 bit data.
 * @param size
 * @return number of bits
 */
int QRinput_estimateBitsMode8(int size)
{
	return size * 8;
}

/**
 * Convert the 8bits data to a bit stream.
 * @param entry
 */
static void QRinput_encodeMode8(QRinput_List *entry, int version)
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
 * @param size
 * @return number of bits
 */
int QRinput_estimateBitsModeKanji(int size)
{
	return (size / 2) * 13;
}

/**
 * Check the input data.
 * @param size
 * @param data
 * @return result
 */
static int QRinput_checkModeKanji(int size, const unsigned char *data)
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
 * Convert the kanji data to a bit stream.
 * @param entry
 */
static void QRinput_encodeModeKanji(QRinput_List *entry, int version)
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
 * Structured Symbol
 *****************************************************************************/

/**
 * Convert a structure symbol code to a bit stream.
 * @param entry
 */
static void QRinput_encodeModeStructure(QRinput_List *entry, int version)
{
	entry->bstream = BitStream_new();

	BitStream_appendNum(entry->bstream, 4, 0x03);
	BitStream_appendNum(entry->bstream, 4, entry->data[1] - 1);
	BitStream_appendNum(entry->bstream, 4, entry->data[0] - 1);
	BitStream_appendNum(entry->bstream, 8, entry->data[2]);
}

/******************************************************************************
 * Validation
 *****************************************************************************/

int QRinput_check(QRencodeMode mode, int size, const unsigned char *data)
{
	if(size <= 0) return -1;

	switch(mode) {
		case QR_MODE_NUM:
			return QRinput_checkModeNum(size, (const char *)data);
			break;
		case QR_MODE_AN:
			return QRinput_checkModeAn(size, (const char *)data);
			break;
		case QR_MODE_KANJI:
			return QRinput_checkModeKanji(size, data);
			break;
		case QR_MODE_8:
			return 0;
			break;
		case QR_MODE_STRUCTURE:
			return 0;
			break;
		default:
			break;
	}

	return -1;
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
static int QRinput_estimateBitStreamSizeOfEntry(QRinput_List *entry, int version)
{
	int bits = 0;
	int l, m;
	int num;

	if(version == 0) version = 1;

	switch(entry->mode) {
		case QR_MODE_NUM:
			bits = QRinput_estimateBitsModeNum(entry->size);
			break;
		case QR_MODE_AN:
			bits = QRinput_estimateBitsModeAn(entry->size);
			break;
		case QR_MODE_8:
			bits = QRinput_estimateBitsMode8(entry->size);
			break;
		case QR_MODE_KANJI:
			bits = QRinput_estimateBitsModeKanji(entry->size);
			break;
		case QR_MODE_STRUCTURE:
			return STRUCTURE_HEADER_BITS;
		default:
			return 0;
	}

	l = QRspec_lengthIndicator(entry->mode, version);
	m = 1 << l;
	num = (entry->size + m - 1) / m;

	bits += num * (4 + l); // mode indicator (4bits) + length indicator

	return bits;
}

/**
 * Estimates the length of the encoded bit stream of the data.
 * @param input input data
 * @param version version of the symbol
 * @return number of bits
 */
int QRinput_estimateBitStreamSize(QRinput *input, int version)
{
	QRinput_List *list;
	int bits = 0;

	list = input->head;
	while(list != NULL) {
		bits += QRinput_estimateBitStreamSizeOfEntry(list, version);
		list = list->next;
	}

	return bits;
}

/**
 * Estimates the required version number of the symbol.
 * @param input input data
 * @return required version number
 */
static int QRinput_estimateVersion(QRinput *input)
{
	int bits;
	int version, prev;

	version = 0;
	do {
		prev = version;
		bits = QRinput_estimateBitStreamSize(input, prev);
		version = QRspec_getMinimumVersion((bits + 7) / 8, input->level);
		if (version < 0) {
			return -1;
		}
	} while (version > prev);

	return version;
}

/**
 * Returns required length in bytes for specified mode, version and bits.
 * @param mode
 * @param version
 * @param bits
 * @return required length of code words in bytes.
 */
int QRinput_lengthOfCode(QRencodeMode mode, int version, int bits)
{
	int payload, size, chunks, remain, maxsize;

	payload = bits - 4 - QRspec_lengthIndicator(mode, version);
	switch(mode) {
		case QR_MODE_NUM:
			chunks = payload / 10;
			remain = payload - chunks * 10;
			size = chunks * 3;
			if(remain >= 7) {
				size += 2;
			} else if(remain >= 4) {
				size += 1;
			}
			break;
		case QR_MODE_AN:
			chunks = payload / 11;
			remain = payload - chunks * 11;
			size = chunks * 2;
			if(remain >= 6) size++;
			break;
		case QR_MODE_8:
			size = payload / 8;
			break;
		case QR_MODE_KANJI:
			size = (payload / 13) * 2;
			break;
		case QR_MODE_STRUCTURE:
			size = payload / 8;
			break;
		default:
			size = 0;
			break;
	}
	maxsize = QRspec_maximumWords(mode, version);
	if(size < 0) size = 0;
	if(size > maxsize) size = maxsize;

	return size;
}

/******************************************************************************
 * Data conversion
 *****************************************************************************/

/**
 * Convert the input data in the data chunk to a bit stream.
 * @param entry
 * @return number of bits
 */
static int QRinput_encodeBitStream(QRinput_List *entry, int version)
{
	int words;
	QRinput_List *st1, *st2;

	if(entry->bstream != NULL) {
		BitStream_free(entry->bstream);
		entry->bstream = NULL;
	}

	words = QRspec_maximumWords(entry->mode, version);
	if(entry->size > words) {
		st1 = QRinput_List_newEntry(entry->mode, words, entry->data);
		st2 = QRinput_List_newEntry(entry->mode, entry->size - words, &entry->data[words]);
		QRinput_encodeBitStream(st1, version);
		QRinput_encodeBitStream(st2, version);
		entry->bstream = BitStream_new();
		BitStream_append(entry->bstream, st1->bstream);
		BitStream_append(entry->bstream, st2->bstream);
		QRinput_List_freeEntry(st1);
		QRinput_List_freeEntry(st2);
	} else {
		switch(entry->mode) {
			case QR_MODE_NUM:
				QRinput_encodeModeNum(entry, version);
				break;
			case QR_MODE_AN:
				QRinput_encodeModeAn(entry, version);
				break;
			case QR_MODE_8:
				QRinput_encodeMode8(entry, version);
				break;
			case QR_MODE_KANJI:
				QRinput_encodeModeKanji(entry, version);
				break;
			case QR_MODE_STRUCTURE:
				QRinput_encodeModeStructure(entry, version);
				break;
			default:
				break;
		}
	}

	return BitStream_size(entry->bstream);
}

/**
 * Convert the input data to a bit stream.
 * @param input input data.
 * @return length of the bit stream.
 */
static int QRinput_createBitStream(QRinput *input)
{
	QRinput_List *list;
	int bits = 0;

	list = input->head;
	while(list != NULL) {
		bits += QRinput_encodeBitStream(list, input->version);
		list = list->next;
	}

	return bits;
}

/**
 * Convert the input data to a bit stream.
 * When the version number is given and that is not sufficient, it is increased
 * automatically.
 * @param input input data.
 * @return -1 if the input data was too large. Otherwise 0.
 */
static int QRinput_convertData(QRinput *input)
{
	int bits;
	int ver;

	ver = QRinput_estimateVersion(input);
	if(ver > QRinput_getVersion(input)) {
		QRinput_setVersion(input, ver);
	}

	for(;;) {
		bits = QRinput_createBitStream(input);
		ver = QRspec_getMinimumVersion((bits + 7) / 8, input->level);
		if(ver < 0) {
			return -1;
		} else if(ver > QRinput_getVersion(input)) {
			QRinput_setVersion(input, ver);
		} else {
			break;
		}
	}

	return 0;
}

/**
 * Create padding bits for the input data.
 * @param input input data.
 * @return padding bit stream.
 */
static BitStream *QRinput_createPaddingBit(QRinput *input)
{
	int bits, maxbits, words, maxwords, i;
	QRinput_List *list;
	BitStream *bstream;

	maxwords = QRspec_getDataLength(input->version, input->level);
	maxbits = maxwords * 8;

	list = input->head;
	bits = 0;
	while(list != NULL) {
		bits += BitStream_size(list->bstream);
		list = list->next;
	}

	words = (bits + 7) / 8;

	if(maxbits - bits < 5) {
		if(maxbits == bits) {
			return NULL;
		} else {
			bstream = BitStream_new();
			BitStream_appendNum(bstream, maxbits - bits, 0);
			return bstream;
		}
	}

	bits += 4;
	words = (bits + 7) / 8;

	bstream = BitStream_new();
	BitStream_appendNum(bstream, words * 8 - bits + 4, 0);

	for(i=0; i<maxwords - words; i++) {
		BitStream_appendNum(bstream, 8, (i&1)?0x11:0xec);
	}

	return bstream;
}

/**
 * Merge all bit streams in the input data.
 * @param input input data.
 * @return merged bit stream
 */

BitStream *QRinput_mergeBitStream(QRinput *input)
{
	BitStream *bstream;
	QRinput_List *list;

	if(QRinput_convertData(input) < 0) {
		return NULL;
	}

	bstream = BitStream_new();
	if(bstream == NULL) return NULL;

	list = input->head;
	while(list != NULL) {
		BitStream_append(bstream, list->bstream);
		list = list->next;
	}

	return bstream;
}

/**
 * Merge all bit streams in the input data and append padding bits
 * @param input input data.
 * @return padded merged bit stream
 */

BitStream *QRinput_getBitStream(QRinput *input)
{
	BitStream *bstream;
	BitStream *padding;

	bstream = QRinput_mergeBitStream(input);
	if(bstream == NULL) {
		return NULL;
	}
	padding = QRinput_createPaddingBit(input);
	if(padding != NULL) {
		BitStream_append(bstream, padding);
		BitStream_free(padding);
	}

	return bstream;
}

/**
 * Pack all bit streams padding bits into a byte array.
 * @param input input data.
 * @return padded merged byte stream
 */

unsigned char *QRinput_getByteStream(QRinput *input)
{
	BitStream *bstream;
	unsigned char *array;

	bstream = QRinput_getBitStream(input);
	if(bstream == NULL) {
		return NULL;
	}
	array = BitStream_toByte(bstream);
	BitStream_free(bstream);

	return array;
}

/******************************************************************************
 * Structured input data
 *****************************************************************************/

static QRinput_InputList *QRinput_InputList_newEntry(QRinput *input)
{
	QRinput_InputList *entry;

	entry = (QRinput_InputList *)malloc(sizeof(QRinput_InputList));
	if(entry == NULL) return NULL;

	entry->input = input;
	entry->next = NULL;

	return entry;
}

static QRinput_InputList *QRinput_InputList_freeEntry(QRinput_InputList *entry)
{
	QRinput_InputList *next;

	next = entry->next;
	QRinput_free(entry->input);
	free(entry);

	return next;
}

QRinput_Struct *QRinput_Struct_new(void)
{
	QRinput_Struct *s;

	s = (QRinput_Struct *)malloc(sizeof(QRinput_Struct));
	if(s == NULL) return NULL;

	s->size = 0;
	s->parity = -1;
	s->head = NULL;
	s->tail = NULL;

	return s;
}

void QRinput_Struct_setParity(QRinput_Struct *s, unsigned char parity)
{
	s->parity = (int)parity;
}

int QRinput_Struct_appendInput(QRinput_Struct *s, QRinput *input)
{
	QRinput_InputList *e;

	e = QRinput_InputList_newEntry(input);
	if(e == NULL) return -1;

	s->size++;
	if(s->tail == NULL) {
		s->head = e;
		s->tail = e;
	} else {
		s->tail->next = e;
		s->tail = e;
	}

	return s->size;
}

void QRinput_Struct_free(QRinput_Struct *s)
{
	QRinput_InputList *list;
	
	list = s->head;
	while(list != NULL) {
		list = QRinput_InputList_freeEntry(list);
	}

	free(s);
}

static unsigned char QRinput_Struct_calcParity(QRinput_Struct *s)
{
	QRinput_InputList *list;
	unsigned char parity = 0;

	list = s->head;
	while(list != NULL) {
		parity ^= QRinput_calcParity(list->input);
		list = list->next;
	}

	QRinput_Struct_setParity(s, parity);

	return parity;
}

static int QRinput_List_shrinkEntry(QRinput_List *entry, int bytes)
{
	unsigned char *data;

	data = (unsigned char *)malloc(bytes);
	if(data == NULL) return -1;

	memcpy(data, entry->data, bytes);
	free(entry->data);
	entry->data = data;
	entry->size = bytes;

	return 0;
}

int QRinput_splitEntry(QRinput_List *entry, int bytes)
{
	QRinput_List *e;
	int ret;

	e = QRinput_List_newEntry(entry->mode, entry->size - bytes, entry->data + bytes);
	if(e == NULL) {
		return -1;
	}

	ret = QRinput_List_shrinkEntry(entry, bytes);
	if(ret < 0) {
		QRinput_List_freeEntry(e);
		return -1;
	}

	e->next = entry->next;
	entry->next = e;

	return 0;
}

QRinput_Struct *QRinput_splitQRinputToStruct(QRinput *input)
{
	QRinput *p;
	QRinput_Struct *s;
	int bits, maxbits, nextbits, bytes;
	QRinput_List *list, *next, *prev;

	s = QRinput_Struct_new();
	if(s == NULL) return NULL;

	input = QRinput_dup(input);
	if(input == NULL) {
		QRinput_Struct_free(s);
		return NULL;
	}

	QRinput_Struct_setParity(s, QRinput_calcParity(input));
	maxbits = QRspec_getDataLength(input->version, input->level) * 8 - STRUCTURE_HEADER_BITS;

	if(maxbits <= 0) {
		QRinput_Struct_free(s);
		return NULL;
	}

	bits = 0;
	list = input->head;
	prev = NULL;
	while(list != NULL) {
		nextbits = QRinput_estimateBitStreamSizeOfEntry(list, input->version);
		if(bits + nextbits <= maxbits) {
			bits += QRinput_encodeBitStream(list, input->version);
			prev = list;
			list = list->next;
		} else {
			bytes = QRinput_lengthOfCode(list->mode, input->version, maxbits - bits);
			if(bytes > 0) {
				/* Splits this entry into 2 entries. */
				QRinput_splitEntry(list, bytes);
				/* First half is the tail of the current input. */
				next = list->next;
				list->next = NULL;
				/* Second half is the head of the next input, p.*/
				p = QRinput_new2(input->version, input->level);
				p->head = next;
				/* Renew QRinput.tail. */
				p->tail = input->tail;
				input->tail = list;
				/* Point to the next entry. */
				prev = list;
				list = next;
			} else {
				/* Current entry will go to the next input. */
				prev->next = NULL;
				p = QRinput_new2(input->version, input->level);
				p->head = list;
				p->tail = input->tail;
				input->tail = prev;
			}
			QRinput_Struct_appendInput(s, input);
			input = p;
			bits = 0;
		}
	}
	QRinput_Struct_appendInput(s, input);
	if(s->size > MAX_STRUCTURED_SYMBOLS) {
		QRinput_Struct_free(s);
		errno = ERANGE;
		return NULL;
	}
	if(QRinput_Struct_insertStructuredAppendHeaders(s) < 0) {
		QRinput_Struct_free(s);
		return NULL;
	}

	return s;
}

int QRinput_Struct_insertStructuredAppendHeaders(QRinput_Struct *s)
{
	int num, i;
	QRinput_InputList *list;

	if(s->parity < 0) {
		QRinput_Struct_calcParity(s);
	}
	num = 0;
	list = s->head;
	while(list != NULL) {
		num++;
		list = list->next;
	}
	i = 1;
	list = s->head;
	while(list != NULL) {
		if(QRinput_insertStructuredAppendHeader(list->input, num, i, s->parity))
			return -1;
		i++;
		list = list->next;
	}

	return 0;
}
