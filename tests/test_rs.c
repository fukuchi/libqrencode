#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../qrtest.h"

/* See pp. 73 of JIS X0510:2004 */
void test_rscode1(void)
{
	QRenc_DataStream *stream;
	QRRawCode *code;
	static char str[8] = "01234567";
	static unsigned char correct[26] = {
		0x10, 0x20, 0x0c, 0x56, 0x61, 0x80, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11,
		0xec, 0x11, 0xec, 0x11, 0xa5, 0x24, 0xd4, 0xc1, 0xed, 0x36, 0xc7, 0x87,
		0x2c, 0x55};

	testStart("RS ecc test");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_NUM, 8, (unsigned char *)str);
	QRenc_setErrorCorrectionLevel(stream, QR_EC_LEVEL_M);
	code = QRraw_new(stream);

	testEnd(memcmp(correct + 16, code->rsblock[0].ecc, 10));
	QRenc_freeData(stream);
	QRraw_free(code);
}

int main(int argc, char **argv)
{
	test_rscode1();

	report();

	return 0;
}
