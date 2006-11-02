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
