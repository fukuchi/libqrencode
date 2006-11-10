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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "qrencode.h"
#include "qrspec.h"
#include "bitstream.h"
#include "datastream.h"
#include "rscode.h"

/******************************************************************************
 * Raw code
 *****************************************************************************/
typedef struct {
	int dataLength;
	unsigned char *data;
	int eccLength;
	unsigned char *ecc;
} RSblock;

typedef struct {
	unsigned char *datacode;
	int blocks;
	RSblock *rsblock;
} QRRawCode;

static void RSblock_init(RSblock *block, int dl, unsigned char *data, int el)
{
	RS *rs;

	block->dataLength = dl;
	block->data = data;
	block->eccLength = el;
	block->ecc = (unsigned char *)malloc(el);

	rs = init_rs(8, 0x11d, 0, 1, el, 255 - dl - el);
	encode_rs_char(rs, data, block->ecc);
}

QRRawCode *QRraw_new(QRenc_DataStream *stream)
{
	QRRawCode *raw;
	int *spec;
	int i;
	RSblock *rsblock;
	unsigned char *p;

	raw = (QRRawCode *)malloc(sizeof(QRRawCode));
	raw->datacode = QRenc_getByteStream(stream);
	spec = QRspec_getEccSpec(stream->version, stream->level);
	raw->blocks = QRspec_rsBlockNum(spec);
	raw->rsblock = (RSblock *)malloc(sizeof(RSblock) * raw->blocks);

	rsblock = raw->rsblock;
	p = raw->datacode;
	for(i=0; i<QRspec_rsBlockNum1(spec); i++) {
		RSblock_init(rsblock, QRspec_rsDataCodes1(spec), p,
						QRspec_rsEccCodes1(spec));
		p += QRspec_rsDataCodes1(spec);
		rsblock++;
	}
	for(i=0; i<QRspec_rsBlockNum2(spec); i++) {
		RSblock_init(rsblock, QRspec_rsDataCodes2(spec), p,
						QRspec_rsEccCodes2(spec));
		p += QRspec_rsDataCodes2(spec);
		rsblock++;
	}

	free(spec);

	return raw;
}

void QRraw_free(QRRawCode *raw)
{
	int i;

	free(raw->datacode);
	for(i=0; i<raw->blocks; i++) {
		free(raw->rsblock[i].ecc);
	}
	free(raw->rsblock);
	free(raw);
}
