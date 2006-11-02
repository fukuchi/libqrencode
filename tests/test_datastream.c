#include <stdio.h>
#include "common.h"

void test_freeDataStream(void)
{
	QRenc_DataStream *stream;
	unsigned char dummy[8];

	testStart("Fee DataStream(always passed, but check valgrind.");
	stream = QRenc_initData();
	QRenc_appendData(stream, QR_MODE_8, 8, dummy);
	QRenc_appendData(stream, QR_MODE_8, 8, dummy);
	QRenc_appendData(stream, QR_MODE_8, 8, dummy);
	QRenc_freeData(stream);
	testEnd(0);
}

int main(int argc, char **argv)
{

	test_freeDataStream();

	report();

	return 0;
}
