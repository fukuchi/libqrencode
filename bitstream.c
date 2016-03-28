/*
 * qrencode - QR Code encoder
 *
 * Binary sequence class.
 * Copyright (C) 2006-2011 Kentaro Fukuchi <kentaro@fukuchi.org>
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

#if HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitstream.h"

BitStream *BitStream_new(void)
{
	BitStream *bstream;

	bstream = (BitStream *)malloc(sizeof(BitStream));
	if(bstream == NULL) return NULL;

	bstream->length = 0;
	bstream->data = NULL;

	return bstream;
}

static int BitStream_allocate(BitStream *bstream, int length)
{
	unsigned char *data;

	if(bstream == NULL) {
		return -1;
	}

	data = (unsigned char *)malloc(length);
	if(data == NULL) {
		return -1;
	}

	if(bstream->data) {
		free(bstream->data);
	}
	bstream->length = length;
	bstream->data = data;

	return 0;
}

static BitStream *BitStream_newFromNum(int bits, unsigned int num)
{
	unsigned int mask;
	int i;
	unsigned char *p;
	BitStream *bstream;

	bstream = BitStream_new();
	if(bstream == NULL) return NULL;

	if(BitStream_allocate(bstream, bits)) {
		BitStream_free(bstream);
		return NULL;
	}

	p = bstream->data;
	mask = 1 << (bits - 1);
	for(i=0; i<bits; i++) {
		if(num & mask) {
			*p = 1;
		} else {
			*p = 0;
		}
		p++;
		mask = mask >> 1;
	}

	return bstream;
}

static BitStream *BitStream_newFromBytes(int size, unsigned char *data)
{
	unsigned char mask;
	int i, j;
	unsigned char *p;
	BitStream *bstream;

	bstream = BitStream_new();
	if(bstream == NULL) return NULL;

	if(BitStream_allocate(bstream, size * 8)) {
		BitStream_free(bstream);
		return NULL;
	}

	p = bstream->data;
	for(i=0; i<size; i++) {
		mask = 0x80;
		for(j=0; j<8; j++) {
			if(data[i] & mask) {
				*p = 1;
			} else {
				*p = 0;
			}
			p++;
			mask = mask >> 1;
		}
	}

	return bstream;
}

int BitStream_append(BitStream *bstream, BitStream *arg)
{
	unsigned char *data;

	if(arg == NULL) {
		return -1;
	}
	if(arg->length == 0) {
		return 0;
	}
	if(bstream->length == 0) {
		if(BitStream_allocate(bstream, arg->length)) {
			return -1;
		}
		memcpy(bstream->data, arg->data, arg->length);
		return 0;
	}

	data = (unsigned char *)malloc(bstream->length + arg->length);
	if(data == NULL) {
		return -1;
	}
	memcpy(data, bstream->data, bstream->length);
	memcpy(data + bstream->length, arg->data, arg->length);

	free(bstream->data);
	bstream->length += arg->length;
	bstream->data = data;

	return 0;
}

int BitStream_appendNum(BitStream *bstream, int bits, unsigned int num)
{
	BitStream *b;
	int ret;

	if(bits == 0) return 0;

	b = BitStream_newFromNum(bits, num);
	if(b == NULL) return -1;

	ret = BitStream_append(bstream, b);
	BitStream_free(b);

	return ret;
}

int BitStream_appendBytes(BitStream *bstream, int size, unsigned char *data)
{
	BitStream *b;
	int ret;

	if(size == 0) return 0;

	b = BitStream_newFromBytes(size, data);
	if(b == NULL) return -1;

	ret = BitStream_append(bstream, b);
	BitStream_free(b);

	return ret;
}

unsigned char *BitStream_toByte(BitStream *bstream)
{
	int i, j, size, bytes, oddbits;
	unsigned char *data, v;
	unsigned char *p;

	size = BitStream_size(bstream);
	if(size == 0) {
		return NULL;
	}
	data = (unsigned char *)malloc((size + 7) / 8);
	if(data == NULL) {
		return NULL;
	}

	bytes = size  / 8;

	p = bstream->data;
	for(i=0; i<bytes; i++) {
		v = 0;
		for(j=0; j<8; j++) {
			v = v << 1;
			v |= *p;
			p++;
		}
		data[i] = v;
	}
	oddbits = size & 7;
	if(oddbits > 0) {
		v = 0;
		for(j=0; j<oddbits; j++) {
			v = v << 1;
			v |= *p;
			p++;
		}
		data[bytes] = v << (8 - oddbits);
	}

	return data;
}

void BitStream_free(BitStream *bstream)
{
	if(bstream != NULL) {
		free(bstream->data);
		free(bstream);
	}
}
