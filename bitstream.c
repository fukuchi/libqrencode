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

#include "bitstream.h"

BitStream *BitStream_new(void)
{
	BitStream *bstream;

	bstream = (BitStream *)malloc(sizeof(BitStream));
	bstream->data = NULL;

	return bstream;
}

BitStream *BitStream_newFromNum(int bits, unsigned int num)
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

BitStream *BitStream_newFromBytes(int size, unsigned char *data)
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
	return strlen(bstream->data);
}

void BitStream_free(BitStream *bstream)
{
	free(bstream->data);
	free(bstream);
}
