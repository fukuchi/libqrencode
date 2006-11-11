/*
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

#ifndef __QRSPEC_H__
#define __QRSPEC_H__

#include "qrencode.h"

/******************************************************************************
 * Version and capacity
 *****************************************************************************/

/**
 * Maximum version (size) of QR-code symbol.
 */
#define QRSPEC_VERSION_MAX 40

/**
 * Return maximum data code length (bytes) for the version.
 * @param version
 * @param level
 * @return maximum size (bytes)
 */
extern int QRspec_getMaximumCodeLength(int version, QRenc_ErrorCorrectionLevel level);

/**
 * Return a version number that satisfies the input code length.
 * @param size input code length (byte)
 * @param level
 * @return version number
 */
extern int QRspec_getMinimumVersion(int size, QRenc_ErrorCorrectionLevel level);

/******************************************************************************
 * Length indicator
 *****************************************************************************/

/**
 * Return the size of lenght indicator for the mode and version.
 * @param mode
 * @param version
 * @return the size of the appropriate length indicator (bits).
 */
extern int QRspec_lengthIndicator(QRenc_EncodeMode mode, int version);

/**
 * Return the maximum length for the mode and version.
 * @param mode
 * @param version
 * @return the maximum length (bytes)
 */
extern int QRspec_maximumWords(QRenc_EncodeMode mode, int version);

/******************************************************************************
 * Error correction code
 *****************************************************************************/

/**
 * Return an array of ECC specification.
 * @param version
 * @param level
 * @return an array of ECC specification contains as following:
 * {# of type1 blocks, # of data code, # of ecc code,
 *  # of type2 blocks, # of data code, # of ecc code}
 * It can be freed by calling free().
 */
int *QRspec_getEccSpec(int version, QRenc_ErrorCorrectionLevel level);

#define QRspec_rsBlockNum(__spec__) (__spec__[0] + __spec__[3])
#define QRspec_rsBlockNum1(__spec__) (__spec__[0])
#define QRspec_rsDataCodes1(__spec__) (__spec__[1])
#define QRspec_rsEccCodes1(__spec__) (__spec__[2])
#define QRspec_rsBlockNum2(__spec__) (__spec__[3])
#define QRspec_rsDataCodes2(__spec__) (__spec__[4])
#define QRspec_rsEccCodes2(__spec__) (__spec__[5])

/******************************************************************************
 * Alignment pattern
 *****************************************************************************/

/**
 * Array of positions of alignment patterns.
 * X and Y coordinates are interleaved into 'pos'.
 */
typedef struct {
	int n;		//< Number of patterns
	int *pos;
} QRspec_Alignment;

/**
 * Return positions of alignment patterns.
 * @param version
 * @return a QRspec_Alignment object that contains all of positions of alignment
 * patterns.
 */
extern QRspec_Alignment *QRspec_getAlignmentPattern(int version);

/**
 * Free QRspec_Alignment instance.
 * @param al QRspec_Alignment instance.
 */
extern void QRspec_freeAlignment(QRspec_Alignment *al);

/******************************************************************************
 * Version information pattern
 *****************************************************************************/

/**
 * Return BCH encoded version information pattern that is used for the symbol
 * of version 7 or greater. Use lower 18 bits.
 * @param version
 * @return BCH coded version information pattern
 */
extern unsigned int QRspec_getVersionPattern(int version);

/******************************************************************************
 * Frame
 *****************************************************************************/

/**
 * Return a copy of initialized frame.
 * @param version
 * @return Array of unsigned char. You can free it by free().
 */
extern unsigned char *QRspec_newFrame(int version);

#endif /* __QRSPEC_H__ */
