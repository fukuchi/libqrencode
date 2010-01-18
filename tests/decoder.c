#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../qrspec.h"
#include "decoder.h"
#include <iconv.h>

enum {
	DECODE_OK = 0,
	DECODE_INVALID = 1,
};

DataChunk *DataChunk_new(QRencodeMode mode)
{
	DataChunk *chunk;

	chunk = (DataChunk *)calloc(1, sizeof(DataChunk));
	if(chunk == NULL) return NULL;

	chunk->mode = mode;

	return chunk;
}

void DataChunk_free(DataChunk *chunk)
{
	if(chunk) {
		if(chunk->data) free(chunk->data);
		free(chunk);
	}
}

DataChunk *decodeNum(int bits_length, unsigned char *bits, int version)
{
	int i, j;
	int lbits, size, words, remain;
	unsigned char *p;
	char *buf, *q;
	int val;
	DataChunk *chunk;

	lbits = QRspec_lengthIndicator(QR_MODE_NUM, version);
	if(bits_length < lbits) {
		fprintf(stderr, "Bit length is too short: %d\n", bits_length);
		return NULL;
	}
	p = bits;
	val = 0;
	for(i=0; i<lbits; i++) {
		val = val << 1;
		val += bits[i];
	}
	size = val;
	p += lbits;

	words = size / 3;
	remain = size - words * 3;
	i = words * 10;
	if(remain == 2) {
		i += 7;
	} else if(remain == 1) {
		i += 4;
	}
	printf("size: %d, words: %d, i: %d\n", size, words, i);
	if(bits_length != i + lbits) {
		fprintf(stderr, "Incorrect bit length: %d, expected %d\n", bits_length, i + lbits);
		return NULL;
	}

	buf = (char *)malloc(size + 1);
	q = buf;
	for(i=0; i<words; i++) {
		val = 0;
		for(j=0; j<10; j++) {
			val = val << 1;
			val += (int)p[j];
		}
		sprintf(q, "%03d", val);
		p += 10;
		q += 3;
	}
	if(remain == 2) {
		val = 0;
		for(j=0; j<7; j++) {
			val = val << 1;
			val += (int)p[j];
		}
		sprintf(q, "%02d", val);
	} else if(remain == 1) {
		val = 0;
		for(j=0; j<4; j++) {
			val = val << 1;
			val += (int)p[j];
		}
		sprintf(q, "%1d", val);
	}

	chunk = DataChunk_new(QR_MODE_NUM);
	chunk->size = size;
	chunk->data = (unsigned char *)buf;

	return chunk;
}

static const char decodeAnTable[45] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', ' ', '$', '%', '*',
	'+', '-', '.', '/', ':'
};

DataChunk *decodeAn(int bits_length, unsigned char *bits, int version)
{
	int i, j;
	int lbits, size, words, remain;
	unsigned char *p;
	char *buf, *q;
	int val;
	int ch, cl;
	DataChunk *chunk;

	lbits = QRspec_lengthIndicator(QR_MODE_AN, version);
	if(bits_length < lbits) {
		fprintf(stderr, "Bit length is too short: %d\n", bits_length);
		return NULL;
	}
	p = bits;
	val = 0;
	for(i=0; i<lbits; i++) {
		val = val << 1;
		val += bits[i];
	}
	size = val;
	p += lbits;

	words = size / 2;
	remain = size - words * 2;
	if(bits_length != words * 11 + remain * 6 + lbits) {
		fprintf(stderr, "Incorrect bit length: %d, expected %d\n", bits_length, words * 11 + remain * 6 + lbits);
		return NULL;
	}

	buf = (char *)malloc(size + 1);
	q = buf;
	for(i=0; i<words; i++) {
		val = 0;
		for(j=0; j<11; j++) {
			val = val << 1;
			val += (int)p[j];
		}
		ch = val / 45;
		cl = val % 45;
		sprintf(q, "%c%c", decodeAnTable[ch], decodeAnTable[cl]);
		p += 11;
		q += 2;
	}
	if(remain == 1) {
		val = 0;
		for(j=0; j<6; j++) {
			val = val << 1;
			val += (int)p[j];
		}
		sprintf(q, "%c", decodeAnTable[val]);
	}

	chunk = DataChunk_new(QR_MODE_AN);
	chunk->size = size;
	chunk->data = (unsigned char *)buf;

	return chunk;
}

DataChunk *decode8(int bits_length, unsigned char *bits, int version)
{
	int i, j;
	int lbits, size;
	unsigned char *p;
	unsigned char *buf, *q;
	int val;
	DataChunk *chunk;

	lbits = QRspec_lengthIndicator(QR_MODE_8, version);
	if(bits_length < lbits) {
		fprintf(stderr, "Bit length is too short: %d\n", bits_length);
		return NULL;
	}
	p = bits;
	val = 0;
	for(i=0; i<lbits; i++) {
		val = val << 1;
		val += bits[i];
	}
	size = val;
	p += j;

	if(bits_length != size * 8 + lbits) {
		fprintf(stderr, "Incorrect bit length: %d, expected %d\n", bits_length,  size * 8 + lbits);
		return NULL;
	}

	buf = (unsigned char *)malloc(size);
	q = buf;
	for(i=0; i<size; i++) {
		val = 0;
		for(j=0; j<8; j++) {
			val = val << 1;
			val += (int)p[8];
		}
		*q = (unsigned char)val;
		p += 8;
		q += 1;
	}

	chunk = DataChunk_new(QR_MODE_8);
	chunk->size = size;
	chunk->data = buf;

	return chunk;
}

DataChunk *decodeKanji(int bits_length, unsigned char *bits, int version)
{
	int i, j;
	int lbits, size;
	unsigned char *p;
	char *buf, *q;
	int val;
	int ch, cl;
	DataChunk *chunk;

	lbits = QRspec_lengthIndicator(QR_MODE_KANJI, version);
	if(bits_length < lbits) {
		fprintf(stderr, "Bit length is too short: %d\n", bits_length);
		return NULL;
	}
	p = bits;
	val = 0;
	for(i=0; i<lbits; i++) {
		val = val << 1;
		val += bits[i];
	}
	size = val;
	p += lbits;

	if(bits_length != size * 13 + lbits) {
		fprintf(stderr, "Incorrect bit length: %d, expected %d\n", bits_length, size * 13 + lbits);
		return NULL;
	}

	buf = (char *)malloc(size * 2 + 1);
	q = buf;
	for(i=0; i<size; i++) {
		val = 0;
		for(j=0; j<13; j++) {
			val = val << 1;
			val += (int)p[j];
		}
		ch = val / 0xc0;
		cl = val - ch * 0xc0;
		val = ch * 256 + cl;
		if(val >= 0x1f00) {
			val += 0xc140;
		} else {
			val += 0x8140;
		}
		sprintf(q, "%c%c", (val>>8) & 0xff, val & 0xff);
		p += 13;
		q += 2;
	}

	chunk = DataChunk_new(QR_MODE_KANJI);
	chunk->size = size * 2;
	chunk->data = (unsigned char *)buf;

	return chunk;
}

DataChunk *decodeChunk(int bits_length, unsigned char *bits, int version)
{
	int i, val;

	if(bits_length < 4) {
		fprintf(stderr, "Bit length too short: %d\n", bits_length);
		return NULL;
	}
	val = 0;
	for(i=0; i<4; i++) {
		val = val << 1;
		val += bits[i];
	}
	printf("Mode: %d\n", val);
	bits_length -= 4;
	bits += 4;
	switch(val) {
		case 1:
			return decodeNum(bits_length, bits, version);
		case 2:
			return decodeAn(bits_length, bits, version);
		case 4:
			return decode8(bits_length, bits, version);
		case 8:
			return decodeKanji(bits_length, bits, version);
		default:
			break;
	}

	fprintf(stderr, "Invalid mode in a chunk: %d\n", val);

	return NULL;
}

void dumpNum(DataChunk *chunk)
{
	printf("%s\n", chunk->data);
}

void dumpAn(DataChunk *chunk)
{
	printf("%s\n", chunk->data);
}

void dump8(DataChunk *chunk)
{
	int i, j;
	unsigned char c;
	int count = 0;
	unsigned char buf[16];

	for(i=0; i<chunk->size; i++) {
		buf[count] = chunk->data[i];
		c = chunk->data[i];
		if(c >= ' ' && c <= '~') {
			putchar(c);
		} else {
			putchar('.');
		}
		count++;

		if(count >= 16) {
			putchar(' ');
			for(j=0; j<16; j++) {
				printf(" %02x", buf[j]);
			}
			count = 0;
			putchar('\n');
		}
	}
	if(count > 0) {
		for(i=0; i<16 - count; i++) {
			putchar(' ');
		}
		putchar(' ');
		for(j=0; j<count; j++) {
			printf(" %02x", buf[j]);
		}
		count = 0;
		putchar('\n');
	}
}

void dumpKanji(DataChunk *chunk)
{
	iconv_t conv;
	char *inbuf, *outbuf, *outp;
	size_t inbytes, outbytes, ret;

	conv = iconv_open("UTF-8", "SHIFT_JIS");
	inbytes = chunk->size;
	inbuf = (char *)chunk->data;
	outbytes = inbytes * 4 + 1;
	outbuf = (char *)malloc(inbytes * 4 + 1);
	outp = outbuf;
	ret = iconv(conv, &inbuf, &inbytes, &outp, &outbytes);
	if(ret < 0) { perror(NULL); }
	*outp = '\0';

	printf("%s\n", outbuf);

	iconv_close(conv);
	free(outbuf);
}

void dumpChunk(DataChunk *chunk)
{
	switch(chunk->mode) {
		case QR_MODE_NUM:
			printf("Numeric: %d bytes\n", chunk->size);
			dumpNum(chunk);
			break;
		case QR_MODE_AN:
			printf("AlphaNumeric: %d bytes\n", chunk->size);
			dumpAn(chunk);
			break;
		case QR_MODE_8:
			printf("8-bit data: %d bytes\n", chunk->size);
			dump8(chunk);
			break;
		case QR_MODE_KANJI:
			printf("Kanji: %d bytes\n", chunk->size);
			dumpKanji(chunk);
			break;
		default:
			printf("Invalid or reserved: %d bytes\n", chunk->size);
			dump8(chunk);
			break;
	}
}

void dumpChunks(QRdata *qrdata)
{
	DataChunk *chunk;

	chunk = qrdata->chunks;
	while(chunk != NULL) {
		dumpChunk(chunk);
		chunk = chunk->next;
	}
}

void concatChunks(QRdata *qrdata)
{
	int idx;
	unsigned char *data;
	DataChunk *chunk;
	int size = 0;

	chunk = qrdata->chunks;
	while(chunk != NULL) {
		size += chunk->size;
		chunk = chunk->next;
	}
	if(size <= 0) {
		qrdata->status = DECODE_INVALID;
		return;
	}

	data = malloc(size);
	chunk = qrdata->chunks;
	idx = 0;
	while(chunk != NULL) {
		memcpy(&data[idx], chunk->data, chunk->size);
		idx += chunk->size;
		chunk = chunk->next;
	}
	qrdata->size = size;
	qrdata->data = data;
}

QRdata *decodeQr(QRcode *code)
{
}
