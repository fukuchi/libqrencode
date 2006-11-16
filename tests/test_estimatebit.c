#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "../datastream.h"
#include "../qrencode_inner.h"

QRinput *gstream;

void test_numbit(void)
{
	QRinput *stream;
	char num[9]="01234567";
	int bits;

	testStart("Estimation of Numeric stream (8 digits)");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_NUM, 8, (unsigned char *)num);
	bits = QRenc_estimateBitStreamSize(stream, 0);
	testEndExp(bits == 41);

	QRenc_appendData(gstream, QR_MODE_NUM, 8, (unsigned char *)num);
	QRenc_freeData(stream);
}

void test_numbit2(void)
{
	QRinput *stream;
	char num[17]="0123456789012345";
	int bits;

	testStart("Estimation of Numeric stream (16 digits)");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_NUM, 16, (unsigned char *)num);
	bits = QRenc_estimateBitStreamSize(stream, 0);
	testEndExp(bits == 68);

	QRenc_appendData(gstream, QR_MODE_NUM, 16, (unsigned char *)num);
	QRenc_freeData(stream);
}

void test_numbit3(void)
{
	QRinput *stream;
	char *num;
	int bits;

	testStart("Estimation of Numeric stream (400 digits)");
	stream = QRenc_newData();
	num = (char *)malloc(401);
	memset(num, '1', 400);
	num[400] = '\0';
	QRenc_appendData(stream, QR_MODE_NUM, 400, (unsigned char *)num);
	bits = QRenc_estimateBitStreamSize(stream, 0);
	testEndExp(bits == 1362);

	QRenc_appendData(gstream, QR_MODE_NUM, 400, (unsigned char *)num);
	QRenc_freeData(stream);
	free(num);
}

void test_an(void)
{
	QRinput *stream;
	char str[6]="AC-42";
	int bits;

	testStart("Estimation of Alphabet-Numeric stream (5 chars)");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_AN, 5, (unsigned char *)str);
	bits = QRenc_estimateBitStreamSize(stream, 0);
	testEndExp(bits == 41);

	QRenc_appendData(gstream, QR_MODE_AN, 5, (unsigned char *)str);
	QRenc_freeData(stream);
}

void test_8(void)
{
	QRinput *stream;
	char str[9]="12345678";
	int bits;

	testStart("Estimation of 8 bit data stream (8 bytes)");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_8, 8, (unsigned char *)str);
	bits = QRenc_estimateBitStreamSize(stream, 0);
	testEndExp(bits == 76);

	QRenc_appendData(gstream, QR_MODE_8, 8, (unsigned char *)str);
	QRenc_freeData(stream);
}

void test_kanji(void)
{
	int res;

	QRinput *stream;
	unsigned char str[4]= {0x93, 0x5f,0xe4, 0xaa};
	int bits;

	testStart("Estimation of Kanji stream (2 chars)");
	stream = QRenc_newData();
	res = QRenc_appendData(stream, QR_MODE_KANJI, 4, (unsigned char *)str);
	if(res < 0) {
		printf("Failed to add.\n");
		testEnd(1);
	} else {
		bits = QRenc_estimateBitStreamSize(stream, 0);
		testEndExp(bits == 38);
		QRenc_appendData(gstream, QR_MODE_KANJI, 4, (unsigned char *)str);
	}

	QRenc_freeData(stream);
}

void test_mix(void)
{
	int bits;

	testStart("Estimation of Mixed stream");
	bits = QRenc_estimateBitStreamSize(gstream, 0);
	testEndExp(bits == (41 + 68 + 1362 + 41 + 76 + 38));
	QRenc_freeData(gstream);
}

int main(int argc, char **argv)
{
	gstream = QRenc_newData();

    test_numbit();
    test_numbit2();
    test_numbit3();
    test_an();
    test_8();
    test_kanji();
	test_mix();

	report();

    return 0;
}
