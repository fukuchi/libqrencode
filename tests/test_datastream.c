#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../qrtest.h"

void test_encodeKanji(void)
{
	QRenc_DataStream *stream;
	unsigned char str[4]= {0x93, 0x5f,0xe4, 0xaa};
	char correct[] = "10000000001001101100111111101010101010";
	BitStream *bstream;

	testStart("Encoding kanji stream.");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_KANJI, 4, (unsigned char *)str);
	bstream = QRenc_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRenc_freeData(stream);
	BitStream_free(bstream);
}

void test_encode8(void)
{
	QRenc_DataStream *stream;
	char str[] = "AC-42";
	char correct[] = "00100000001010011100111011100111001000010";
	BitStream *bstream;

	testStart("Encoding alphabet-numeric stream.");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_AN, 5, (unsigned char *)str);
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

	testStart("Encoding INVALID alphabet-numeric stream.");
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

	testStart("Encoding numeric stream. (8 digits)");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_NUM, 8, (unsigned char *)num);
	bstream = QRenc_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRenc_freeData(stream);
	BitStream_free(bstream);
}

void test_encodeNumericPadded(void)
{
	QRenc_DataStream *stream;
	char num[9] = "01234567";
	char correct[] = "000100000010000000001100010101100110000110000000";
	BitStream *bstream;
	int flag;

	testStart("Encoding numeric stream. (8 digits)(padded)");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_NUM, 8, (unsigned char *)num);
	bstream = QRenc_getBitStream(stream);
	flag = strncmp(correct, bstream->data, 48);
	if(strlen(bstream->data) != 208)
		flag++;
	testEnd(flag);

	QRenc_freeData(stream);
	BitStream_free(bstream);
}

void test_encodeNumeric2(void)
{
	QRenc_DataStream *stream;
	char num[] = "0123456789012345";
	char correct[] = "00010000010000000000110001010110011010100110111000010100111010100101";
	BitStream *bstream;

	testStart("Encoding numeric stream. (16 digits)");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_NUM, 16, (unsigned char *)num);
	bstream = QRenc_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRenc_freeData(stream);
	BitStream_free(bstream);
}

void test_encode82(void)
{
	QRenc_DataStream *stream;
	unsigned char *data;
	BitStream *bstream;
	char c1[] = "010011111111";
	char c2[] = "010000000010";
	int flag = 0;

	data = (unsigned char *)malloc(257);
	memset(data, 0, 257);
	testStart("Encoding byte stream. (257 bytes)");
	stream = QRenc_newData();
	QRenc_appendData(stream, QR_MODE_8, 257, data);
	bstream = QRenc_mergeBitStream(stream);
	if(strncmp(c1, bstream->data, 12)) {
		flag++;
	}
	if(strncmp(c2, bstream->data + 12 + 255*8, 12)) {
		flag++;
	}
	testEnd(flag);
	QRenc_freeData(stream);
	BitStream_free(bstream);
	free(data);
}

int main(int argc, char **argv)
{
	test_encodeNumeric();
	test_encodeNumeric2();
	test_encode8();
	test_encode82();
	test_encodeAn();
	test_encodeAn2();
	test_encodeKanji();
	test_encodeNumericPadded();

	report();

	return 0;
}
