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
	int size;				///< Size of data chunk.
	unsigned char *data;	///< Data chunk.
	BitStream *bstream;
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
 * Convert the data stream in the data chunk to a bit stream.
 * @param list
 * @return number of bits
 */
static int QRenc_encodeBitStream(QRenc_List *list)
{
	assert(list != NULL);
	switch(list->mode) {
		case QR_MODE_NUM:
//			return QRenc_encodeModeNum(list);
			break;
		case QR_MODE_AN:
//			return QRenc_encodeModeAn(list);
			break;
		case QR_MODE_8:
//			return QRenc_encodeMode8(list);
			break;
		case QR_MODE_KANJI:
//			return QRenc_encodeModeKanji(list);
			break;
	}
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
		BitStream_append(bstream, list->bstream->size, list->bstream->data);
	}

	return bstream;
}
