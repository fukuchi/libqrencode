#ifndef __DECODER_H__
#define __DECODER_H__

#include "../qrencode.h"

typedef struct _DataChunk {
	QRencodeMode mode;
	int size;
	int bits;
	unsigned char *data;
	struct _DataChunk *next;
} DataChunk;

typedef struct {
	int size;
	unsigned char *data;
	int version;
	QRecLevel level;
	DataChunk *chunks, *last;
} QRdata;

QRdata *QRdata_new(void);
int QRdata_decodeBitStream(QRdata *qrdata, BitStream *bstream);
void QRdata_dump(QRdata *data);
void QRdata_free(QRdata *data);

unsigned int QRcode_decodeVersion(QRcode *code);
int QRcode_decodeFormat(QRcode *code, QRecLevel *level, int *mask);
unsigned char *QRcode_unmask(QRcode *code);
unsigned char *QRcode_extractBits(QRcode *code, int *length);
QRdata *QRcode_decodeBits(QRcode *code);
QRdata *QRcode_decode(QRcode *code);

#endif /* __DECODER_H__ */
