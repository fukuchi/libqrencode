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

#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

typedef struct {
	int size;
	unsigned char *data;

	int remain;
	unsigned char *head;
} BitStream;

extern BitStream *BitStream_new(int size);
void BitStream_append(BitStream *bstream, int size, unsigned char *data);
void BitStream_free(BitStream *bstream);

#define BitStream_appendBitStream(__arg1__, __arg2__) \
	BitStream_append((__arg1__), ((__arg2__)->size), ((__arg2__)->data))

#endif /* __BITSTREAM_H__ */
