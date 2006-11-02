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

#include "qrencode.h"

typedef struct _QRenc_List QRenc_List;
struct _QRenc_List {
	QRenc_EncodeMode mode;
	int size;
	unsigned char *data;
	QRenc_List *next;
};

struct _QRenc_DataStream {
	QRenc_List *head;
	QRenc_List *tail;
};

static QRenc_ErrorCorrectionLevel errorCorrectionLevel = QR_EC_LEVEL_L;
static int version = 0;


void QRenc_setErrorCorrectionLevel(QRenc_ErrorCorrectionLevel level)
{
	errorCorrectionLevel = level;
}

QRenc_ErrorCorrectionLevel QRenc_getErrorCorrectionLevel(void)
{
	return errorCorrectionLevel;
}

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
		free(list);
		list = next;
	}

	free(stream);
}
