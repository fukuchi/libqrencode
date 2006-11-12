#include "qrencode.h"
#include "bitstream.h"

typedef struct {
	int width; //< Edge length of the symbol
	int words;   //< Data capacity (bytes)
	int remainder; //< Remainder bit (bits)
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
	int count;
	int dataLength;
	int eccLength;
	int b1;
	int b2;
} QRRawCode;

extern QRRawCode *QRraw_new(QRenc_DataStream *stream);
extern unsigned char QRraw_getCode(QRRawCode *raw);
extern void QRraw_free(QRRawCode *raw);

unsigned char *QRenc_fillerTest(int version);
unsigned char *QRenc_mask(int width, unsigned char *frame, int mask);
