#include "qrencode.h"
#include "bitstream.h"

extern int QRenc_estimateBitStreamSize(QRenc_DataStream *stream, int version);
extern BitStream *QRenc_mergeBitStream(QRenc_DataStream *stream);
extern int QRenc_estimateVersion(QRenc_DataStream *stream);
