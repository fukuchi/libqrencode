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

#ifndef __QRSPEC_H__
#define __QRSPEC_H__

#include "qrencode.h"

/******************************************************************************
 * Version and capacity
 *****************************************************************************/

#define QRSPEC_VERSION_MAX 40

typedef struct {
	int length; //< Edge length of the symbol
	int words;   //< Data capacity (bytes)
	int rsL;
	int rsM;
	int rsH;
	int rsQ;
} QRspec_Capacity;

extern QRspec_Capacity qrspecCapacity[];

/**
 * Return a version number that satisfies the input code length.
 * @param size input code length (byte)
 * @return version number
 */
extern int QRspec_getMinVersion(int size);

/******************************************************************************
 * Length indicator
 *****************************************************************************/

/**
 * Return the size of lenght indicator for mode and version.
 * @param mode
 * @param version
 * @return the size of the appropriate length indicator (bits).
 */
extern int QRspec_lengthIndicator(QRenc_EncodeMode mode, int version);

#endif /* __QRSPEC_H__ */
