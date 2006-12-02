#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../qrinput.h"
#include "../qrencode_inner.h"

void test_encodeKanji(void)
{
	QRinput *stream;
	unsigned char str[4]= {0x93, 0x5f,0xe4, 0xaa};
	char correct[] = "10000000001001101100111111101010101010";
	BitStream *bstream;

	testStart("Encoding kanji stream.");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_KANJI, 4, (unsigned char *)str);
	bstream = QRinput_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encode8(void)
{
	QRinput *stream;
	char str[] = "AC-42";
	char correct[] = "00100000001010011100111011100111001000010";
	BitStream *bstream;

	testStart("Encoding alphabet-numeric stream.");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_AN, 5, (unsigned char *)str);
	bstream = QRinput_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encodeAn(void)
{
	QRinput *stream;
	char str[] = "AC-42";
	char correct[] = "00100000001010011100111011100111001000010";
	BitStream *bstream;

	testStart("Encoding alphabet-numeric stream.");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_AN, 5, (unsigned char *)str);
	bstream = QRinput_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encodeAn2(void)
{
	QRinput *stream;
	char str[] = "!,;$%";
	int ret;

	testStart("Encoding INVALID alphabet-numeric stream.");
	stream = QRinput_new();
	ret = QRinput_append(stream, QR_MODE_AN, 5, (unsigned char *)str);
	testEnd(!ret);
	QRinput_free(stream);
}

void test_encodeNumeric(void)
{
	QRinput *stream;
	char num[9] = "01234567";
	char correct[] = "00010000001000000000110001010110011000011";
	BitStream *bstream;

	testStart("Encoding numeric stream. (8 digits)");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_NUM, 8, (unsigned char *)num);
	bstream = QRinput_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encodeNumericPadded(void)
{
	QRinput *stream;
	char num[9] = "01234567";
	char correct[] = "000100000010000000001100010101100110000110000000";
	BitStream *bstream;
	int flag;

	testStart("Encoding numeric stream. (8 digits)(padded)");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_NUM, 8, (unsigned char *)num);
	bstream = QRinput_getBitStream(stream);
	flag = strncmp(correct, bstream->data, 48);
	if(strlen(bstream->data) != 19 * 8)
		flag |= 0x80;
	testEnd(flag);

	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encodeNumericPadded2(void)
{
	QRinput *stream;
	char num[8] = "0123456";
	char correct[] = "000100000001110000001100010101100101100000000000";
	BitStream *bstream;
	int flag;

	testStart("Encoding numeric stream. (7 digits)(padded)");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_NUM, 7, (unsigned char *)num);
	bstream = QRinput_getBitStream(stream);
	flag = strncmp(correct, bstream->data, 48);
	if(strlen(bstream->data) != 19 * 8)
		flag |= 0x80;
	testEnd(flag);

	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encodeNumeric2(void)
{
	QRinput *stream;
	char num[] = "0123456789012345";
	char correct[] = "00010000010000000000110001010110011010100110111000010100111010100101";
	BitStream *bstream;

	testStart("Encoding numeric stream. (16 digits)");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_NUM, 16, (unsigned char *)num);
	bstream = QRinput_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encodeNumeric3(void)
{
	QRinput *stream;
	char num[9] = "0123456";
	char correct[] = "0001""0000000111""0000001100""0101011001""0110";
	BitStream *bstream;

	testStart("Encoding numeric stream. (7 digits)");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_NUM, 7, (unsigned char *)num);
	bstream = QRinput_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(strcmp(correct, bstream->data));
	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encodeTooLong(void)
{
	QRinput *stream;
	unsigned char *data;
	BitStream *bstream;

	data = (unsigned char *)malloc(7089);
	memset(data, 'a', 7089);

	testStart("Encoding long string. (7089 bytes)");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_AN, 7089, data);
	bstream = QRinput_mergeBitStream(stream);
	testEndExp(bstream == NULL);
	QRinput_free(stream);
	if(bstream != NULL) {
		BitStream_free(bstream);
	}
	free(data);
}

void test_encodeAnNum(void)
{
	QRinput *input;
	BitStream *bstream;

	testStart("Bit length check of alpha-numeric stream. (11 + 12)");
	input = QRinput_new();
	QRinput_append(input, QR_MODE_AN, 11, (unsigned char *)"abcdefghijk");
	QRinput_append(input, QR_MODE_NUM, 12, (unsigned char *)"123456789012");
	bstream = QRinput_mergeBitStream(input);
	testEndExp(strlen(bstream->data) == 128);
	QRinput_free(input);
	BitStream_free(bstream);

	testStart("Bit length check of alphabet stream. (23)");
	input = QRinput_new();
	QRinput_append(input, QR_MODE_AN, 23, (unsigned char *)"abcdefghijk123456789012");
	bstream = QRinput_mergeBitStream(input);
	testEndExp(strlen(bstream->data) == 140);
	QRinput_free(input);
	BitStream_free(bstream);
}

int main(int argc, char **argv)
{
	test_encodeNumeric();
	test_encodeNumeric2();
	test_encodeNumeric3();
	test_encode8();
	test_encodeTooLong();
	test_encodeAn();
	test_encodeAn2();
	test_encodeKanji();
	test_encodeNumericPadded();
	test_encodeNumericPadded2();
	test_encodeAnNum();

	report();

	return 0;
}
