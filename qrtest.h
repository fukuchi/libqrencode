#include "qrencode.h"
#include "bitstream.h"

extern int QRenc_estimateBitStreamSize(QRenc_DataStream *stream, int version);
extern BitStream *QRenc_mergeBitStream(QRenc_DataStream *stream);
extern BitStream *QRenc_getBitStream(QRenc_DataStream *stream);

extern int *QRspec_getEccBlockNum(int version, QRenc_ErrorCorrectionLevel level);
