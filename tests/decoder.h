#ifndef __DECODER_H__
#define __DECODER_H__

#include "../qrencode.h"

typedef struct _DataChunk {
	QRencodeMode mode;
	int size;
	unsigned char *data;
	struct _DataChunk *next;
} DataChunk;

typedef struct {
	int status;
	int size;
	unsigned char *data;
	int version;
	QRecLevel level;
	DataChunk *chunks;
} QRdata;

DataChunk *decodeNum(int bits_length, unsigned char *bits, int version);
DataChunk *decodeAn(int bits_length, unsigned char *bits, int version);
DataChunk *decode8(int bits_length, unsigned char *bits, int version);
DataChunk *decodeKanji(int bits_length, unsigned char *bits, int version);
DataChunk *decodeChunk(int bits_length, unsigned char *bits, int version);
void dumpChunk(DataChunk *chunk);
QRdata *decode(QRcode *code);

#endif /* __DECODER_H__ */
