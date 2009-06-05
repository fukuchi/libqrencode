#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "common.h"
#include "../qrspec.h"
#include "../qrinput.h"
#include "../mask.h"
#include "../split.h"
#include "../bitstream.h"

static int inputTest(QRinput_List *list, const char *fmt, ...)
{
	va_list ap;
	int size;
	QRencodeMode mode;
	int i, err = 0;

	va_start(ap, fmt);
	i = 1;
	while(*fmt) {
		if(list == NULL) {
			err = 1;
			break;
		}
		size = va_arg(ap, int);
		if(list->size != size) {
			err = 1;
			break;
		}

		switch(*fmt++) {
		case 'n':
			mode = QR_MODE_NUM;
			break;
		case 'a':
			mode = QR_MODE_AN;
			break;
		case 'k':
			mode = QR_MODE_KANJI;
			break;
		case '8':
			mode = QR_MODE_8;
			break;
		default:
			return -1;
			break;
		}
		if(list->mode != mode) {
			err = 1;
			break;
		}
		list = list->next;
		i++;
	}
	va_end(ap);
	if(list != NULL) {
		err = 1;
	}
	if(err) {
		return -i;
	}
	return 0;
}

int inputSize(QRinput *input)
{
	BitStream *bstream;
	int size;

	bstream = QRinput_mergeBitStream(input);
	size = BitStream_size(bstream);
	BitStream_free(bstream);

	return size;
}

void test_split1(void)
{
	QRinput *input;
	BitStream *stream;

	testStart("Split test: null string");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("", input, QR_MODE_8, 0);
	stream = QRinput_mergeBitStream(input);
	testEndExp(BitStream_size(stream) == 0);
	QRinput_free(input);
	BitStream_free(stream);
}

void test_split2(void)
{
	QRinput *input;
	QRinput_List *list;
	int err = 0;

	testStart("Split test: single typed strings (num)");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("0123", input, QR_MODE_8, 0);
	list = input->head;
	if(inputTest(list, "n", 4)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);

	err = 0;
	testStart("Split test: single typed strings (num2)");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("12345678901234567890", input, QR_MODE_KANJI, 0);
	list = input->head;
	if(inputTest(list, "n", 20)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);
}

void test_split3(void)
{
	QRinput *input;
	QRinput_List *list;
	int err = 0;

	testStart("Split test: single typed strings (an)");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("ab:-E", input, QR_MODE_8, 0);
	list = input->head;
	if(inputTest(list, "a", 5)) {
		printQRinputInfo(input);
		err++;
	}
	testEnd(err);
	QRinput_free(input);

	err = 0;
	testStart("Split test: num + an");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("0123abcde", input, QR_MODE_KANJI, 0);
	list = input->head;
	if(inputTest(list, "a", 9)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);

	err = 0;
	testStart("Split test: an + num + an");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("Ab345fg", input, QR_MODE_KANJI, 0);
	list = input->head;
	if(inputTest(list, "a", 7)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);
}

void test_split4(void)
{
	QRinput *input;
	QRinput *i1, *i2;
	int s1, s2, size;
#define CHUNKA "ABCDEFGHIJK"
#define CHUNKB "123456"
#define CHUNKC "1234567"

	testStart("Split test: an and num entries");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput(CHUNKA/**/CHUNKB, input, QR_MODE_8, 0);
	i1 = QRinput_new();
	QRinput_append(i1, QR_MODE_AN, 17, (unsigned char *)CHUNKA/**/CHUNKB);
	i2 = QRinput_new();
	QRinput_append(i2, QR_MODE_AN, 11, (unsigned char *)CHUNKA);
	QRinput_append(i2, QR_MODE_NUM, 6, (unsigned char *)CHUNKB);

	size = inputSize(input);
	s1 = inputSize(i1);
	s2 = inputSize(i2);
	testEndExp(size == ((s1 < s2)?s1:s2));
	QRinput_free(input);
	QRinput_free(i1);
	QRinput_free(i2);

	testStart("Split test: num and an entries");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput(CHUNKB/**/CHUNKA, input, QR_MODE_8, 0);
	i1 = QRinput_new();
	QRinput_append(i1, QR_MODE_AN, 17, (unsigned char *)CHUNKB/**/CHUNKA);
	i2 = QRinput_new();
	QRinput_append(i2, QR_MODE_NUM, 6, (unsigned char *)CHUNKB);
	QRinput_append(i2, QR_MODE_AN, 11, (unsigned char *)CHUNKA);

	size = inputSize(input);
	s1 = inputSize(i1);
	s2 = inputSize(i2);
	testEndExp(size == ((s1 < s2)?s1:s2));
	QRinput_free(input);
	QRinput_free(i1);
	QRinput_free(i2);

	testStart("Split test: num and an entries (should be splitted)");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput(CHUNKC/**/CHUNKA, input, QR_MODE_8, 0);
	i1 = QRinput_new();
	QRinput_append(i1, QR_MODE_AN, 18, (unsigned char *)CHUNKC/**/CHUNKA);
	i2 = QRinput_new();
	QRinput_append(i2, QR_MODE_NUM, 7, (unsigned char *)CHUNKC);
	QRinput_append(i2, QR_MODE_AN, 11, (unsigned char *)CHUNKA);

	size = inputSize(input);
	s1 = inputSize(i1);
	s2 = inputSize(i2);
	testEndExp(size == ((s1 < s2)?s1:s2));
	QRinput_free(input);
	QRinput_free(i1);
	QRinput_free(i2);
}

void test_split5(void)
{
	QRinput *input;
	QRinput_List *list;
	int err = 0;

	testStart("Split test: bit, an, bit, num");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("\x82\xd9""abcdeabcdea\x82\xb0""123456", input, QR_MODE_8, 0);
	list = input->head;
	if(inputTest(list, "8a8n", 2, 11, 2, 6)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);
}

void test_split6(void)
{
	QRinput *input;
	QRinput_List *list;
	int err = 0;

	testStart("Split test: kanji, an, kanji, num");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("\x82\xd9""abcdeabcdea\x82\xb0""123456", input, QR_MODE_KANJI, 0);
	list = input->head;
	if(inputTest(list, "kakn", 2, 11, 2, 6)) {
		printQRinputInfo(input);
		err++;
	}
	testEnd(err);
	QRinput_free(input);
}

void test_split7(void)
{
	QRinput *input;
	QRinput_List *list;
	int err = 0;

	testStart("Split test: an and num as bits");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("\x82\xd9""abcde\x82\xb0""12345", input, QR_MODE_8, 0);
	list = input->head;
	if(inputTest(list, "8n", 9, 5)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);
}

void test_split8(void)
{
	QRinput *input;
	QRinput_List *list;
	int err = 0;

	testStart("Split test: terminated with a half of kanji code");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("\x82\xd9""abcdefgh\x82", input, QR_MODE_KANJI, 0);
	list = input->head;
	if(inputTest(list, "ka8", 2, 8, 1)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);
}

void test_split3c(void)
{
	QRinput *input;
	QRinput_List *list;
	int err = 0;

	testStart("Split test: single typed strings (an, case-sensitive)");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("ab:-E", input, QR_MODE_8, 1);
	list = input->head;
	if(inputTest(list, "8", 5)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);

	err = 0;
	testStart("Split test: num + an");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("0123abcde", input, QR_MODE_KANJI, 1);
	list = input->head;
	if(inputTest(list, "n8", 4, 5)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);

	err = 0;
	testStart("Split test: an + num + an");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("Ab345fg", input, QR_MODE_KANJI, 1);
	list = input->head;
	if(inputTest(list, "8", 7)) {
		printQRinputInfo(input);
		err++;
	}
	testEnd(err);
	QRinput_free(input);
}

void test_toupper(void)
{
	QRinput *input;
	QRinput_List *list;
	int err = 0;

	testStart("Split test: check dupAndToUpper (lower->upper)");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("abcde", input, QR_MODE_8, 0);
	list = input->head;
	if(inputTest(list, "a", 5)) {
		err++;
	}
	if(strncmp((char *)list->data, "ABCDE", list->size)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);

	err = 0;
	testStart("Split test: check dupAndToUpper (kanji)");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("\x83n\x83q\x83t\x83w\x83z", input, QR_MODE_KANJI, 0);
	list = input->head;
	if(inputTest(list, "k", 10)) {
		printQRinputInfo(input);
		err++;
	}
	if(strncmp((char *)list->data, "\x83n\x83q\x83t\x83w\x83z", list->size)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);

	err = 0;
	testStart("Split test: check dupAndToUpper (8bit)");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("\x83n\x83q\x83t\x83w\x83z", input, QR_MODE_8, 0);
	list = input->head;
	if(inputTest(list, "8", 10)) {
		printQRinputInfo(input);
		err++;
	}
	if(strncmp((char *)list->data, "\x83N\x83Q\x83T\x83W\x83Z", list->size)) {
		err++;
	}
	testEnd(err);
	QRinput_free(input);
}

void test_splitNum8(void)
{
	QRinput *input;
	QRinput_List *list;
	int err = 0;

	testStart("Split test: num and 8bit to 8bit");
	input = QRinput_new2(0, QR_ECLEVEL_L);
	Split_splitStringToQRinput("1abcdefg", input, QR_MODE_8, 1);
	list = input->head;
	if(inputTest(list, "8", 8)) {
		err++;
		printQRinputInfo(input);
	}
	testEnd(err);
	QRinput_free(input);
}

int main(void)
{
	test_split1();
	test_split2();
	test_split3();
	test_split4();
	test_split5();
	test_split6();
	test_split7();
	test_split8();
	test_split3c();
	test_toupper();
	test_splitNum8();

	report();

	return 0;
}
