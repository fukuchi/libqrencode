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

#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

typedef struct {
	char *data;
} BitStream;

extern BitStream *BitStream_new(void);
extern void BitStream_append(BitStream *bstream, BitStream *arg);
extern void BitStream_appendNum(BitStream *bstream, int bits, unsigned int num);
extern void BitStream_appendBytes(BitStream *bstream, int size, unsigned char *data);
extern unsigned int BitStream_size(BitStream *bstream);
extern unsigned char *BitStream_toByte(BitStream *bstream);
extern void BitStream_free(BitStream *bstream);

#endif /* __BITSTREAM_H__ */
