#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../qrtest.h"

void test_encodeNumeric(void)
{
	QRenc_DataStream *stream;
	char num[9] = "01234567";
	char correct[] = "00010000001000000000110001010110011000011";
	BitStream *bstream;

	testStart("Encoding numeric stream.");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_NUM, 8, (unsigned char *)num);
	QRenc_setVersion(QRenc_estimateVersion(stream));
	bstream = QRenc_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
}

int main(int argc, char **argv)
{
	test_encodeNumeric();

	report();

	return 0;
}
