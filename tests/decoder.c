#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include "../qrspec.h"
#include "../bitstream.h"
#include "../mask.h"
#include "../mqrspec.h"
#include "../mmask.h"
#include "decoder.h"

static unsigned int bitToInt(unsigned char *bits, int length)
{
	int i;
	unsigned int val = 0;

	for(i=0; i<length; i++) {
		val = val << 1;
		val |= (bits[i] & 1);
	}

	return val;
}

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

static int decodeLength(int *bits_length, unsigned char **bits, QRencodeMode mode, int version, int mqr)
{
	int i;
	int length = 0;
	int lbits;
	
	if(mqr) {
		lbits = MQRspec_lengthIndicator(mode, version);
	} else {
		lbits = QRspec_lengthIndicator(mode, version);
	}

	if(*bits_length < lbits) {
		printf("Bit length is too short: %d\n", *bits_length);
		return 0;
	}

	length = 0;
	for(i=0; i<lbits; i++) {
		length = length << 1;
		length += (*bits)[i];
	}

	*bits_length -= lbits;
	*bits += lbits;

	return length;
}

static DataChunk *decodeNum(int *bits_length, unsigned char **bits, int version, int mqr)
{
	int i;
	int size, sizeInBit, words, remain;
	unsigned char *p;
	char *buf, *q;
	unsigned int val;
	DataChunk *chunk;

	size = decodeLength(bits_length, bits, QR_MODE_NUM, version, mqr);
	if(size < 0) return NULL;

	words = size / 3;
	remain = size - words * 3;
	sizeInBit = words * 10;
	if(remain == 2) {
		sizeInBit += 7;
	} else if(remain == 1) {
		sizeInBit += 4;
	}
	if(*bits_length < sizeInBit) {
		printf("Bit length is too short: %d, expected %d.\n", *bits_length, sizeInBit);
		return NULL;
	}

	buf = (char *)malloc(size + 1);
	p = *bits;
	q = buf;
	for(i=0; i<words; i++) {
		val = bitToInt(p, 10);
		sprintf(q, "%03d", val);
		p += 10;
		q += 3;
	}
	if(remain == 2) {
		val = bitToInt(p, 7);
		sprintf(q, "%02d", val);
	} else if(remain == 1) {
		val = bitToInt(p, 4);
		sprintf(q, "%1d", val);
	}
	buf[size] = '\0';

	chunk = DataChunk_new(QR_MODE_NUM);
	chunk->size = size;
	chunk->data = (unsigned char *)buf;
	*bits_length -= sizeInBit;
	*bits += sizeInBit;

	return chunk;
}

static const char decodeAnTable[45] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', ' ', '$', '%', '*',
	'+', '-', '.', '/', ':'
};

static DataChunk *decodeAn(int *bits_length, unsigned char **bits, int version, int mqr)
{
	int i;
	int size, sizeInBit, words, remain;
	unsigned char *p;
	char *buf, *q;
	unsigned int val;
	int ch, cl;
	DataChunk *chunk;

	size = decodeLength(bits_length, bits, QR_MODE_AN, version, mqr);
	if(size < 0) return NULL;

	words = size / 2;
	remain = size - words * 2;
	sizeInBit = words * 11 + remain * 6;
	if(*bits_length < sizeInBit) {
		printf("Bit length is too short: %d, expected %d.\n", *bits_length, sizeInBit);
		return NULL;
	}

	buf = (char *)malloc(size + 1);
	p = *bits;
	q = buf;
	for(i=0; i<words; i++) {
		val = bitToInt(p, 11);
		ch = val / 45;
		cl = val % 45;
		sprintf(q, "%c%c", decodeAnTable[ch], decodeAnTable[cl]);
		p += 11;
		q += 2;
	}
	if(remain == 1) {
		val = bitToInt(p, 6);
		sprintf(q, "%c", decodeAnTable[val]);
	}

	chunk = DataChunk_new(QR_MODE_AN);
	chunk->size = size;
	chunk->data = (unsigned char *)buf;
	*bits_length -= sizeInBit;
	*bits += sizeInBit;

	return chunk;
}

static DataChunk *decode8(int *bits_length, unsigned char **bits, int version, int mqr)
{
	int i;
	int size, sizeInBit;
	unsigned char *p;
	unsigned char *buf, *q;
	DataChunk *chunk;

	size = decodeLength(bits_length, bits, QR_MODE_8, version, mqr);
	if(size < 0) return NULL;

	sizeInBit = size * 8;
	if(*bits_length < sizeInBit) {
		printf("Bit length is too short: %d, expected %d.\n", *bits_length, sizeInBit);
		return NULL;
	}

	buf = (unsigned char *)malloc(size);
	p = *bits;
	q = buf;
	for(i=0; i<size; i++) {
		*q = (unsigned char)bitToInt(p, 8);
		p += 8;
		q += 1;
	}

	chunk = DataChunk_new(QR_MODE_8);
	chunk->size = size;
	chunk->data = buf;
	*bits_length -= sizeInBit;
	*bits += sizeInBit;

	return chunk;
}

static DataChunk *decodeKanji(int *bits_length, unsigned char **bits, int version, int mqr)
{
	int i;
	int size, sizeInBit;
	unsigned char *p;
	char *buf, *q;
	unsigned int val;
	int ch, cl;
	DataChunk *chunk;

	size = decodeLength(bits_length, bits, QR_MODE_KANJI, version, mqr);
	if(size < 0) return NULL;

	sizeInBit = size * 13;
	if(*bits_length < sizeInBit) {
		printf("Bit length is too short: %d, expected %d.\n", *bits_length, sizeInBit);
		return NULL;
	}

	buf = (char *)malloc(size * 2 + 1);
	p = *bits;
	q = buf;
	for(i=0; i<size; i++) {
		val = bitToInt(p, 13);
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
	*bits_length -= sizeInBit;
	*bits += sizeInBit;

	return chunk;
}

static DataChunk *decodeChunk(int *bits_length, unsigned char **bits, int version)
{
	int val;

	if(*bits_length < 4) {
		return NULL;
	}
	val = bitToInt(*bits, 4);;
	*bits_length -= 4;
	*bits += 4;
	switch(val) {
		case 0:
			return NULL;
		case 1:
			return decodeNum(bits_length, bits, version, 0);
		case 2:
			return decodeAn(bits_length, bits, version, 0);
		case 4:
			return decode8(bits_length, bits, version, 0);
		case 8:
			return decodeKanji(bits_length, bits, version, 0);
		default:
			break;
	}

	printf("Invalid mode in a chunk: %d\n", val);

	return NULL;
}

static DataChunk *decodeChunkMQR(int *bits_length, unsigned char **bits, int version)
{
	int modebits, termbits;
	unsigned int val;

	modebits = version - 1;
	termbits = version * 2 + 1;
	if(*bits_length >= termbits) {
		val = bitToInt(*bits, termbits);
		if(val == 0) {
			*bits += termbits;
			*bits_length -= termbits;
			return NULL;
		}
	} else {
		if(*bits_length < modebits) {
			val = bitToInt(*bits, *bits_length);
		} else {
			val = bitToInt(*bits, modebits);
		}
		if(val == 0) {
			return NULL;
		} else {
			printf("Terminating bits include 1-bit.\n");
			return NULL;
		}
	}
	val = bitToInt(*bits, modebits);
	if(version == 4 && val > 3) {
		printf("Invalid mode number %d.\n", val);
	}
	*bits_length -= modebits;
	*bits += modebits;
	switch(val) {
		case 0:
			return decodeNum(bits_length, bits, version, 1);
		case 1:
			return decodeAn(bits_length, bits, version, 1);
		case 2:
			return decode8(bits_length, bits, version, 1);
		case 3:
			return decodeKanji(bits_length, bits, version, 1);
		default:
			break;
	}

	printf("Invalid mode in a chunk: %d\n", val);

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
	if(ret == (size_t) -1) { perror(NULL); }
	*outp = '\0';

	printf("%s\n", outbuf);

	iconv_close(conv);
	free(outbuf);
}

static void dumpChunk(DataChunk *chunk)
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

void QRdata_concatChunks(QRdata *qrdata)
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
		return;
	}

	data = malloc(size + 1);
	chunk = qrdata->chunks;
	idx = 0;
	while(chunk != NULL) {
		memcpy(&data[idx], chunk->data, chunk->size);
		idx += chunk->size;
		chunk = chunk->next;
	}
	data[size] = '\0';
	qrdata->size = size;
	qrdata->data = data;
}

int appendChunk(QRdata *qrdata, int *bits_length, unsigned char **bits)
{
	DataChunk *chunk;

	if(qrdata->mqr) {
		chunk = decodeChunkMQR(bits_length, bits, qrdata->version);
	} else {
		chunk = decodeChunk(bits_length, bits, qrdata->version);
	}
	if(chunk == NULL) {
		return 1;
	}

	if(qrdata->last == NULL) {
		qrdata->chunks = chunk;
	} else {
		qrdata->last->next = chunk;
	}
	qrdata->last = chunk;

	return 0;
}

QRdata *QRdata_new(void)
{
	QRdata *qrdata;

	qrdata = (QRdata *)calloc(sizeof(QRdata), 1);
	if(qrdata == NULL) return NULL;

	return qrdata;
}

QRdata *QRdata_newMQR(void)
{
	QRdata *qrdata;

	qrdata = (QRdata *)calloc(sizeof(QRdata), 1);
	if(qrdata == NULL) return NULL;
	qrdata->mqr = 1;

	return qrdata;
}

void QRdata_free(QRdata *qrdata)
{
	DataChunk *chunk, *next;

	chunk = qrdata->chunks;
	while(chunk != NULL) {
		next = chunk->next;
		DataChunk_free(chunk);
		chunk = next;
	}

	if(qrdata->data != NULL) {
		free(qrdata->data);
	}
	free(qrdata);
}

static int QRdata_decodeBits(QRdata *qrdata, int length, unsigned char *bits)
{
	int ret = 0;

	while(ret == 0) {
		ret = appendChunk(qrdata, &length, &bits);
	}

	return length;
}

int QRdata_decodeBitStream(QRdata *qrdata, BitStream *bstream)
{
	return QRdata_decodeBits(qrdata, bstream->length, bstream->data);
}

void QRdata_dump(QRdata *data)
{
	dumpChunks(data);
}

unsigned int QRcode_decodeVersion(QRcode *code)
{
	unsigned int v1, v2;
	int x, y, width;
	unsigned char *p;

	width = code->width;
	if(width < 45) {
		return (width - 21)/ 4 + 1;
	}

	v1 = 0;
	p = code->data + width * (width - 9) + 5;
	for(x=0; x<6; x++) {
		for(y=0; y<3; y++) {
			v1 = v1 << 1;
			v1 |= *(p - y * width - x) & 1;
		}
	}

	v2 = 0;
	p = code->data + width * 5 + width - 9;
	for(y=0; y<6; y++) {
		for(x=0; x<3; x++) {
			v2 = v2 << 1;
			v2 |= *(p - y * width - x) & 1;
		}
	}

	if(v1 != v2) {
		printf("Two verion patterns are different.\n");
		return -1;
	}

	return v1 >> 12;
}

int QRcode_decodeFormat(QRcode *code, QRecLevel *level, int *mask)
{
	unsigned int v1, v2;
	int i, width;
	unsigned char *p;

	width = code->width;

	v1 = 0;
	p = code->data + width * 8;
	for(i=0; i<8; i++) {
		v1 = v1 << 1;
		if(i < 6) {
			v1 |= *(p + i) & 1;
		} else {
			v1 |= *(p + i + 1) & 1;
		}
	}
	p = code->data + width * 7 + 8;
	for(i=0; i<7; i++) {
		v1 = v1 << 1;
		if(i < 1) {
			v1 |= *(p - width * i) & 1;
		} else {
			v1 |= *(p - width * (i + 1)) & 1;
		}
	}

	v2 = 0;
	p = code->data + width * (width - 1) + 8;
	for(i=0; i<7; i++) {
		v2 = v2 << 1;
		v2 |= *(p - width * i) & 1;
	}
	p = code->data + width * 8 + width - 8;
	for(i=0; i<8; i++) {
		v2 = v2 << 1;
		v2 |= *(p + i) & 1;
	}

	if(v1 != v2) {
		printf("Two format infos are different.\n");
		return -1;
	}
	v1 = (v1 ^ 0x5412) >> 10;
	*mask = v1 & 7;
	switch((v1 >> 3) & 3) {
		case 1:
			*level = QR_ECLEVEL_L;
			break;
		case 0:
			*level = QR_ECLEVEL_M;
			break;
		case 3:
			*level = QR_ECLEVEL_Q;
			break;
		case 2:
			*level = QR_ECLEVEL_H;
			break;
		default:
			break;
	}

	return 0;
}

static unsigned char *unmask(QRcode *code, QRecLevel level, int mask)
{
	unsigned char *unmasked;

	unmasked = Mask_makeMask(code->width, code->data, mask, level);

	return unmasked;
}

unsigned char *QRcode_unmask(QRcode *code)
{
	int ret, version, mask;
	QRecLevel level;

	version = QRcode_decodeVersion(code);
	if(version < 1) return NULL;
	ret = QRcode_decodeFormat(code, &level, &mask);
	if(ret < 0) return NULL;

	return unmask(code, level, mask);
}

typedef struct {
	int width;
	unsigned char *frame;
	int x, y;
	int dir;
	int bit;
	int mqr;
} FrameFiller;

static FrameFiller *FrameFiller_new(int width, unsigned char *frame, int mqr)
{
	FrameFiller *filler;

	filler = (FrameFiller *)malloc(sizeof(FrameFiller));
	if(filler == NULL) return NULL;
	filler->width = width;
	filler->frame = frame;
	filler->x = width - 1;
	filler->y = width - 1;
	filler->dir = -1;
	filler->bit = -1;
	filler->mqr = mqr;

	return filler;
}

static unsigned char *FrameFiller_next(FrameFiller *filler)
{
	unsigned char *p;
	int x, y, w;

	if(filler->bit == -1) {
		filler->bit = 0;
		return filler->frame + filler->y * filler->width + filler->x;
	}

	x = filler->x;
	y = filler->y;
	p = filler->frame;
	w = filler->width;

	if(filler->bit == 0) {
		x--;
		filler->bit++;
	} else {
		x++;
		y += filler->dir;
		filler->bit--;
	}

	if(filler->dir < 0) {
		if(y < 0) {
			y = 0;
			x -= 2;
			filler->dir = 1;
			if(!filler->mqr && x == 6) {
				x--;
				y = 9;
			}
		}
	} else {
		if(y == w) {
			y = w - 1;
			x -= 2;
			filler->dir = -1;
			if(!filler->mqr && x == 6) {
				x--;
				y -= 8;
			}
		}
	}
	if(x < 0 || y < 0) return NULL;

	filler->x = x;
	filler->y = y;

	if(p[y * w + x] & 0x80) {
		// This tail recursion could be optimized.
		return FrameFiller_next(filler);
	}
	return &p[y * w + x];
}

static unsigned char *extractBits(int width, unsigned char *frame, int spec[5])
{
	unsigned char *bits, *p, *q;
	FrameFiller *filler;
	int i, j;
	int col, row, d1, b1, blocks, idx, words;

	blocks = QRspec_rsBlockNum(spec);
	words = QRspec_rsDataLength(spec);
	d1 = QRspec_rsDataCodes1(spec);
	b1 = QRspec_rsBlockNum1(spec);
	bits = (unsigned char *)malloc(words * 8);
	/*
	 * 00 01 02 03 04 05 06 07 08 09
	 * 10 11 12 13 14 15 16 17 18 19
	 * 20 21 22 23 24 25 26 27 28 29 30
	 * 31 32 33 34 35 36 37 38 39 40 41
	 * 42 43 44 45 46 47 48 49 50 51 52
	 */

	row = col = 0;
	filler = FrameFiller_new(width, frame, 0);
	for(i=0; i<words; i++) {
		col = i / blocks;
		row = i % blocks + ((col >= d1)?b1:0);
		idx = d1 * row + col + ((row > b1)?(row-b1):0);
		q = bits + idx * 8;
		for(j=0; j<8; j++) {
			p = FrameFiller_next(filler);
			q[j] = *p & 1;
		}
	}
	free(filler);

	return bits;
}

unsigned char *QRcode_extractBits(QRcode *code, int *length)
{
	unsigned char *unmasked, *bits;
	int spec[5];
	int ret, version, mask;
	QRecLevel level;

	version = QRcode_decodeVersion(code);
	if(version < 1) return NULL;
	ret = QRcode_decodeFormat(code, &level, &mask);
	if(ret < 0) return NULL;

	QRspec_getEccSpec(version, level, spec);
	*length = QRspec_rsDataLength(spec) * 8;

	unmasked = unmask(code, level, mask);
	if(unmasked == NULL) return NULL;

	bits = extractBits(code->width, unmasked, spec);
	free(unmasked);

	return bits;
}

static void printBits(int length, unsigned char *bits)
{
	int i;

	for(i=0; i<length; i++) {
		putchar((bits[i]&1)?'1':'0');
	}
	putchar('\n');
}

static int checkRemainderWords(int length, unsigned char *bits, int remainder)
{
	int rbits, words;
	unsigned char *p, v;
	int i;

	words = remainder / 8;
	rbits = remainder - words * 8;
	bits += (length - remainder);
	for(i=0; i<rbits; i++) {
		if((bits[i]&1) != 0) {
			printf("Terminating code includes 1-bit.\n");
			printBits(remainder, bits);
			return -1;
		}
	}
	p = bits + rbits;
	for(i=0; i<words; i++) {
		v = (unsigned char)bitToInt(p, 8);
		if(v != ((i&1)?0x11:0xec)) {
			printf("Remainder codewords wrong.\n");
			printBits(remainder, bits);
			return -1;
		}
		p += 8;
	}

	return 0;
}

QRdata *QRcode_decodeBits(QRcode *code)
{
	unsigned char *unmasked, *bits;
	int spec[5];
	int ret, version, mask;
	int length;
	QRecLevel level;
	QRdata *qrdata;

	version = QRcode_decodeVersion(code);
	if(version < 1) return NULL;
	ret = QRcode_decodeFormat(code, &level, &mask);
	if(ret < 0) return NULL;

	QRspec_getEccSpec(version, level, spec);
	length = QRspec_rsDataLength(spec) * 8;

	unmasked = unmask(code, level, mask);
	if(unmasked == NULL) return NULL;

	bits = extractBits(code->width, unmasked, spec);
	free(unmasked);

	qrdata = QRdata_new();
	qrdata->version = version;
	qrdata->level = level;
	ret = QRdata_decodeBits(qrdata, length, bits);
	if(ret > 0) {
		checkRemainderWords(length, bits, ret);
	}

	free(bits);

	return qrdata;
}

QRdata *QRcode_decode(QRcode *code)
{
	QRdata *qrdata;

	qrdata = QRcode_decodeBits(code);
	QRdata_concatChunks(qrdata);

	return qrdata;
}

/*
 * Micro QR Code decoder
 */

struct FormatInfo MQRformat[] = {
	{1, QR_ECLEVEL_L},
	{2, QR_ECLEVEL_L},
	{2, QR_ECLEVEL_M},
	{3, QR_ECLEVEL_L},
	{3, QR_ECLEVEL_M},
	{4, QR_ECLEVEL_L},
	{4, QR_ECLEVEL_M},
	{4, QR_ECLEVEL_Q}
};

int QRcode_decodeFormatMQR(QRcode *code, int *version, QRecLevel *level, int *mask)
{
	unsigned int v, t;
	int i, width;
	unsigned char *p;

	width = code->width;

	v = 0;
	p = code->data + width * 8 + 1;
	for(i=0; i<8; i++) {
		v = v << 1;
		v |= p[i] & 1;
	}
	p = code->data + width * 7 + 8;
	for(i=0; i<7; i++) {
		v = v << 1;
		v |= *(p - width * i) & 1;
	}
	v ^= 0x4445;
	*mask = (v >> 10) & 3;
	t = (v >> 12) & 7;
	*version = MQRformat[t].version;
	*level = MQRformat[t].level;
	if(*version * 2 + 9 != width) {
		printf("Decoded version number does not match to the size.\n");
		return -1;
	}
	return 0;
}

static unsigned char *unmaskMQR(QRcode *code, QRecLevel level, int mask)
{
	unsigned char *unmasked;

	unmasked = MMask_makeMask(code->version, code->data, mask, level);

	return unmasked;
}

unsigned char *QRcode_unmaskMQR(QRcode *code)
{
	int ret, version, mask;
	QRecLevel level;

	ret = QRcode_decodeFormatMQR(code, &version, &level, &mask);
	if(ret < 0) return NULL;

	return unmaskMQR(code, level, mask);
}

static unsigned char *extractBitsMQR(int width, unsigned char *frame, int version, QRecLevel level)
{
	unsigned char *bits;
	FrameFiller *filler;
	int i;
	int size;

	size = MQRspec_getDataLengthBit(version, level);
	bits = (unsigned char *)malloc(size);
	filler = FrameFiller_new(width, frame, 1);
	for(i=0; i<size; i++) {
		bits[i] = *(FrameFiller_next(filler)) & 1;
	}
	free(filler);

	return bits;
}

unsigned char *MQRcode_extractBits(QRcode *code, int *length)
{
	unsigned char *unmasked, *bits;
	int ret, version, mask;
	QRecLevel level;

	ret = QRcode_decodeFormatMQR(code, &version, &level, &mask);
	if(ret < 0) return NULL;

	unmasked = unmaskMQR(code, level, mask);
	if(unmasked == NULL) return NULL;

	*length = MQRspec_getDataLengthBit(version, level);
	bits = extractBitsMQR(code->width, unmasked, version, level);
	free(unmasked);

	return bits;
}

static int checkRemainderWordsMQR(int length, unsigned char *bits, int remainder, int version)
{
	int rbits, words, paddings;
	unsigned char *p, v;
	int i, decoded;

	decoded = length - remainder;
	bits += decoded;
	words = (decoded + 7) / 8;
	rbits = words * 8 - decoded;
	for(i=0; i<rbits; i++) {
		if((bits[i]&1) != 0) {
			printf("Terminating code includes 1-bit.\n");
			printBits(remainder, bits);
			return -1;
		}
	}

	paddings = (length - words * 8) / 8;
	p = bits + rbits;
	for(i=0; i<paddings; i++) {
		v = (unsigned char)bitToInt(p, 8);
		if(v != ((i&1)?0x11:0xec)) {
			printf("Remainder codewords wrong.\n");
			printBits(remainder, bits);
			return -1;
		}
		p += 8;
	}
	rbits = length - (paddings + words)* 8;
	if(rbits > 0) {
		if((version == 1 || version == 3) && rbits == 4) {
			v = (unsigned char)bitToInt(p, 4);
			if(v != 0) {
				printf("Last padding bits include 1-bit.\n");
				return -1;
			}
		} else {
			printf("The length of the last padding bits is %d, not %d.\n", rbits, (version == 1 || version == 3)?4:0);
			return -1;
		}
	}

	return 0;
}

QRdata *QRcode_decodeBitsMQR(QRcode *code)
{
	unsigned char *unmasked, *bits;
	int ret, version, mask;
	int length;
	QRecLevel level;
	QRdata *qrdata;

	ret = QRcode_decodeFormatMQR(code, &version, &level, &mask);
	if(ret < 0) return NULL;

	unmasked = unmaskMQR(code, level, mask);
	if(unmasked == NULL) return NULL;

	length = MQRspec_getDataLengthBit(version, level);
	bits = extractBitsMQR(code->width, unmasked, version, level);
	free(unmasked);

	qrdata = QRdata_newMQR();
	qrdata->version = version;
	qrdata->level = level;
	ret = QRdata_decodeBits(qrdata, length, bits);
	if(ret > 0) {
		checkRemainderWordsMQR(length, bits, ret, version);
	}

	free(bits);

	return qrdata;
}

QRdata *QRcode_decodeMQR(QRcode *code)
{
	QRdata *qrdata;
	qrdata = QRcode_decodeBitsMQR(code);
	QRdata_concatChunks(qrdata);

	return qrdata;
}
