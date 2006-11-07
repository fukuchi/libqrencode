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
	char *data;
} BitStream;

extern BitStream *BitStream_new(void);
extern BitStream *BitStream_newFromNum(int bits, unsigned int num);
extern BitStream *BitStream_newFromBytes(int size, unsigned char *data);
extern void BitStream_append(BitStream *bstream, BitStream *arg);
extern void BitStream_appendNum(BitStream *bstream, int bits, unsigned int num);
extern void BitStream_appendBytes(BitStream *bstream, int size, unsigned char *data);
extern unsigned int BitStream_size(BitStream *bstream);
extern void BitStream_free(BitStream *bstream);

#endif /* __BITSTREAM_H__ */
