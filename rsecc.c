/*
 * qrencode - QR Code encoder
 *
 * Reed solomon error correction code encoder specialized for QR code.
 *
 * Copyright (C) 2013 Kentaro Fukuchi <kentaro@fukuchi.org>
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
#include <stdlib.h>
#include <string.h>

#include "rsecc.h"

static int initialized = 0;

#define SYMBOL_SIZE (8)
#define symbols ((1 << SYMBOL_SIZE) - 1)
static const int proot = 0x11d; /* stands for x^8+x^4+x^3+x^2+1 (see pp.37 of JIS X0510:2004) */

/* min/max codeword length of ECC, calculated from the specification. */
#define min_length (2)
#define max_length (30)
#define max_generatorSize (max_length)

static unsigned char alpha[symbols + 1];
static unsigned char aindex[symbols + 1];
static unsigned char generator[max_length - min_length + 1][max_generatorSize + 1];
static unsigned char generatorInitialized[max_length - min_length + 1];

static void RSECC_initLookupTable(void)
{
	int i, b;

	alpha[symbols] = 0;
	aindex[0] = symbols;

	b = 1;
	for(i = 0; i < symbols; i++) {
		alpha[i] = b;
		aindex[b] = i;
		b <<= 1;
		if(b & (symbols + 1)) {
			b ^= proot;
		}
		b &= symbols;
	}
}

void RSECC_init(void)
{
	RSECC_initLookupTable();
	memset(generatorInitialized, 0, (max_length - min_length + 1));
	initialized = 1;
}

static void generator_init(int length)
{
	int i, j, a;
	int g[max_generatorSize + 1];

	g[0] = 1;
	a = 0;
	for(i = 1; i <= length; i++) {
		g[i] = 1;
		for(j = i - 1; j > 0; j--) {
			if(g[0] != 0) {
				g[j] = g[j - 1] ^  alpha[(aindex[g[j]] + a) % symbols];
			} else {
				g[j] = g[j - 1];
			}
		}
		g[0] = alpha[(aindex[g[0]] + a) % symbols];
		a++;
	}

	for(i = 0; i <= length; i++) {
		generator[length - min_length][i] = aindex[g[i]];
	}

	generatorInitialized[length - min_length] = 1;
}

int RSECC_encode(int datalength, int ecclength, const unsigned char *data, unsigned char *ecc)
{
	int i, j;
	unsigned char feedback;
	unsigned char *gen;

	if(!initialized) {
		RSECC_init();
	}

	if(ecclength > max_length) return -1;

	memset(ecc, 0, ecclength);
	if(!generatorInitialized[ecclength - min_length]) generator_init(ecclength);
	gen = generator[ecclength - min_length];

	for(i = 0; i < datalength; i++) {
		feedback = aindex[data[i] ^ ecc[0]];
		if(feedback != symbols) {
			for(j = 1; j < ecclength; j++) {
				ecc[j] ^= alpha[(feedback + gen[ecclength - j]) % symbols];
			}
		}
		memmove(&ecc[0], &ecc[1], ecclength - 1);
		if(feedback != symbols) {
			ecc[ecclength - 1] = alpha[(feedback + gen[0]) % symbols];
		} else {
			ecc[ecclength - 1] = 0;
		}
	}

	return 0;
}
