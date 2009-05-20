#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../qrencode_inner.h"
#include "../qrinput.h"
#include "../rscode.h"

/* See pp. 73 of JIS X0510:2004 */
void test_rscode1(void)
{
	QRinput *stream;
	QRRawCode *code;
	static const char str[9] = "01234567";
	static unsigned char correct[26] = {
		0x10, 0x20, 0x0c, 0x56, 0x61, 0x80, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11,
		0xec, 0x11, 0xec, 0x11, 0xa5, 0x24, 0xd4, 0xc1, 0xed, 0x36, 0xc7, 0x87,
		0x2c, 0x55};

	testStart("RS ecc test");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_NUM, 8, (unsigned char *)str);
	QRinput_setErrorCorrectionLevel(stream, QR_ECLEVEL_M);
	code = QRraw_new(stream);

	testEnd(memcmp(correct + 16, code->rsblock[0].ecc, 10));
	QRinput_free(stream);
	QRraw_free(code);
}

int main(void)
{
	test_rscode1();

	free_rs_cache();
	report();

	return 0;
}
