#include "qrencode.h"
#include "bitstream.h"

typedef struct {
	int length; //< Edge length of the symbol
	int words;   //< Data capacity (bytes)
	int ec[4];
} QRspec_Capacity;

extern QRspec_Capacity qrspecCapacity[];


extern int QRenc_estimateBitStreamSize(QRenc_DataStream *stream, int version);
extern BitStream *QRenc_mergeBitStream(QRenc_DataStream *stream);
extern BitStream *QRenc_getBitStream(QRenc_DataStream *stream);

typedef struct {
	int dataLength;
	unsigned char *data;
	int eccLength;
	unsigned char *ecc;
} RSblock;

typedef struct {
	unsigned char *datacode;
	int blocks;
	RSblock *rsblock;
} QRRawCode;
extern QRRawCode *QRraw_new(QRenc_DataStream *stream);
extern void QRraw_free(QRRawCode *raw);
