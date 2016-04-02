#ifndef __DATACHUNK_H__
#define __DATACHUNK_H__

#include "../qrencode.h"

typedef struct _DataChunk {
	QRencodeMode mode;
	int size;
	int bits;
	unsigned char *data;
	struct _DataChunk *next;
} DataChunk;

DataChunk *DataChunk_new(QRencodeMode mode);
void DataChunk_free(DataChunk *chunk);
void DataChunk_dumpChunkList(DataChunk *list);

#endif /* __DATACHUNK_H__ */
