/*
 * qrencode - QR Code encoder
 *
 * Binary sequence class.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitstream.h"

BitStream *BitStream_new(void)
{
	BitStream *bstream;

	bstream = (BitStream *)malloc(sizeof(BitStream));
	bstream->data = NULL;

	return bstream;
}

static BitStream *BitStream_newFromNum(int bits, unsigned int num)
{
	unsigned int mask;
	int i;
	char *p;
	BitStream *bstream;

	bstream = BitStream_new();
	bstream->data = (char *)malloc(bits + 1);

	p = bstream->data;
	mask = 1 << (bits - 1);
	for(i=0; i<bits; i++) {
		if(num & mask) {
			*p = '1';
		} else {
			*p = '0';
		}
		p++;
		mask = mask >> 1;
	}
	*p = '\0';

	return bstream;
}

static BitStream *BitStream_newFromBytes(int size, unsigned char *data)
{
	unsigned char mask;
	int i, j;
	char *p;
	BitStream *bstream;

	bstream = BitStream_new();
	bstream->data = (char *)malloc(size * 8 + 1);

	p = bstream->data;
	for(i=0; i<size; i++) {
		mask = 0x80;
		for(j=0; j<8; j++) {
			if(data[i] & mask) {
				*p = '1';
			} else {
				*p = '0';
			}
			p++;
			mask = mask >> 1;
		}
	}
	*p = '\0';

	return bstream;
}

void BitStream_append(BitStream *bstream, BitStream *arg)
{
	int l1, l2;
	char *new;

	if(arg == NULL || arg->data == NULL) {
		return;
	}
	if(bstream->data == NULL) {
		bstream->data = strdup(arg->data);
		return;
	}

	l1 = strlen(bstream->data);
	l2 = strlen(arg->data);
	new = (char *)malloc(l1 + l2 + 1);
	strcpy(new, bstream->data);
	strcat(new, arg->data);

	free(bstream->data);
	bstream->data = new;
}

void BitStream_appendNum(BitStream *bstream, int bits, unsigned int num)
{
	BitStream *b;

	b = BitStream_newFromNum(bits, num);
	BitStream_append(bstream, b);
	BitStream_free(b);
}

void BitStream_appendBytes(BitStream *bstream, int size, unsigned char *data)
{
	BitStream *b;

	b = BitStream_newFromBytes(size, data);
	BitStream_append(bstream, b);
	BitStream_free(b);
}

unsigned int BitStream_size(BitStream *bstream)
{
	if(bstream->data == NULL) return 0;

	return strlen(bstream->data);
}

unsigned char *BitStream_toByte(BitStream *bstream)
{
	int i, j, size, bytes;
	unsigned char *data, v;
	char *p;

	size = BitStream_size(bstream);
	data = (unsigned char *)malloc((size + 7) / 8);
	bytes = size  / 8;

	p = bstream->data;
	for(i=0; i<bytes; i++) {
		v = 0;
		for(j=0; j<8; j++) {
			v = v << 1;
			v |= *p == '1';
			p++;
		}
		data[i] = v;
	}
	if(size & 7) {
		v = 0;
		for(j=0; j<(size & 7); j++) {
			v = v << 1;
			v |= *p == '1';
			p++;
		}
		data[bytes] = v;
	}

	return data;
}

void BitStream_free(BitStream *bstream)
{
	if(bstream->data != NULL) {
		free(bstream->data);
	}
	free(bstream);
}
