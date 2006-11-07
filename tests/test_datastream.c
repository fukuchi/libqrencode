#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../qrtest.h"

void test_encode8(void)
{
	QRenc_DataStream *stream;
	char str[] = "AC-42";
	char correct[] = "00100000001010011100111011100111001000010";
	BitStream *bstream;

	testStart("Encoding alphabet-numeric stream.");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_AN, 5, (unsigned char *)str);
	QRenc_setVersion(stream, QRenc_estimateVersion(stream));
	bstream = QRenc_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRenc_freeData(stream);
	BitStream_free(bstream);
}

void test_encodeAn(void)
{
	QRenc_DataStream *stream;
	char str[] = "AC-42";
	char correct[] = "00100000001010011100111011100111001000010";
	BitStream *bstream;

	testStart("Encoding alphabet-numeric stream.");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_AN, 5, (unsigned char *)str);
	QRenc_setVersion(stream, QRenc_estimateVersion(stream));
	bstream = QRenc_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRenc_freeData(stream);
	BitStream_free(bstream);
}

void test_encodeAn2(void)
{
	QRenc_DataStream *stream;
	char str[] = "!,;$%";
	int ret;

	testStart("Encoding alphabet-numeric stream.");
	stream = QRenc_newData();
	ret = QRenc_appendData(stream, QR_MODE_AN, 5, (unsigned char *)str);
	testEnd(!ret);
	QRenc_freeData(stream);
}

void test_encodeNumeric(void)
{
	QRenc_DataStream *stream;
	char num[9] = "01234567";
	char correct[] = "00010000001000000000110001010110011000011";
	BitStream *bstream;

	testStart("Encoding numeric stream.");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_NUM, 8, (unsigned char *)num);
	QRenc_setVersion(stream, QRenc_estimateVersion(stream));
	bstream = QRenc_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRenc_freeData(stream);
	BitStream_free(bstream);
}

void test_encodeNumeric2(void)
{
	QRenc_DataStream *stream;
	char num[] = "0123456789012345";
	char correct[] = "00010000010000000000110001010110011010100110111000010100111010100101";
	BitStream *bstream;

	testStart("Encoding numeric stream.");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_NUM, 16, (unsigned char *)num);
	QRenc_setVersion(stream, QRenc_estimateVersion(stream));
	bstream = QRenc_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRenc_freeData(stream);
	BitStream_free(bstream);
}

int main(int argc, char **argv)
{
	test_encodeNumeric();
	test_encodeNumeric2();
	test_encodeAn();
	test_encodeAn2();

	report();

	return 0;
}
