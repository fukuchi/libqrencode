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
	int size;				///< Size of data chunk.
	unsigned char *data;	///< Data chunk.
	int bits;				///< Number of bits of encoded bit stream.
	unsigned char *bdata;	///< Encoded bit stream.
	QRenc_List *next;
};

struct _QRenc_DataStream {
	QRenc_List *head;
	QRenc_List *tail;
};


QRenc_DataStream *QRenc_initData(void)
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
	entry->bits = 0;
	entry->bdata = NULL;

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
		if(list->bdata != NULL) {
			free(list->bdata);
		}
		free(list);
		list = next;
	}

	free(stream);
}

/******************************************************************************
 * Data conversion
 *****************************************************************************/

typedef struct {
	int size;
	unsigned char *data;

	int remain;
	unsigned char *head;
} BitStream;

BitStream *BitStream_new(int size)
{
	BitStream *bstream;
	div_t d;
	int byte;

	d = div(size, 8);
	byte = d.quot + d.rem?1:0;

	bstream = (BitStream *)malloc(sizeof(BitStream));
	bstream->size = size;
	bstream->data = (unsigned char *)malloc(byte);
	bstream->head = bstream->data;
	bstream->remain = 0;

	return bstream;
}

void BitStream_append(BitStream *bstream, int size, unsigned char *data)
{
	int sb;
	int shift;

	sb = size / 8;

	if(bstream->remain == 0) {
		memcpy(bstream->head, data, sb);
		bstream->head += sb;
		if(size & 7) {
			*bstream->head = data[sb];
			bstream->remain = 8 - (size - sb * 8);
		}
	} else {
		shift = 8 - bstream->remain;
		while(size > 7) {
			*bstream->head++ |= *data << shift;
			*bstream->head = *data++ >> bstream->remain;
			size -= 8;
		}
		if(size > 0) {
			*bstream->head |= *data << shift;
			if(size > bstream->remain) {
				bstream->head++;
				*bstream->head = *data >> bstream->remain;
				bstream->remain += 8 - size;
			} else {
				bstream->remain -= size;
				if(bstream->remain == 0) {
					bstream->head++;
					bstream->remain = 8;
				}
			}
		}
	}
}

void BitStream_free(BitStream *bstream)
{
	free(bstream->data);
	free(bstream);
}

/**
 * Convert the input data stream to a bit stream.
 * @param stream input data stream.
 * @return bit stream.
 */
static BitStream *QRenc_createBitStream(QRenc_DataStream *stream)
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

	bstream = BitStream_new(bits);
	list = stream->head;
	while(list != NULL) {
		BitStream_append(bstream, list->bits, list->bdata);
	}

	return bstream;
}
