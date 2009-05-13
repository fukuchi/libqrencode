#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "common.h"
#include "../qrinput.h"
#include "../qrencode_inner.h"
#include "../split.h"

void test_encodeKanji(void)
{
	QRinput *stream;
	unsigned char str[4]= {0x93, 0x5f,0xe4, 0xaa};
	unsigned char *buf;
	char correct[] = "10000000001001101100111111101010101010";
	BitStream *bstream;

	testStart("Encoding kanji stream.");
	buf = (unsigned char *)malloc(4);
	memcpy(buf, str, 4);
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_KANJI, 4, buf);
	bstream = QRinput_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(cmpBin(correct, bstream));
	QRinput_free(stream);
	BitStream_free(bstream);
	free(buf);
}

void test_encode8(void)
{
	QRinput *stream;
	char str[] = "AC-42";
	char correct[] = "0100000001010100000101000011001011010011010000110010";
	BitStream *bstream;

	testStart("Encoding 8bit stream.");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_8, 5, (unsigned char *)str);
	bstream = QRinput_mergeBitStream(stream);
	printf("%s\n", correct);
	printf("%s\n", bstream->data);
	testEnd(cmpBin(correct, bstream));
	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encode8_versionup(void)
{
	QRinput *stream;
	BitStream *bstream;
	char *str;
	int version;

	testStart("Encoding 8bit stream. (auto-version up test)");
	str = (char *)malloc(2900);
	memset(str, 0xff, 2900);
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_8, 2900, (unsigned char *)str);
	bstream = QRinput_mergeBitStream(stream);
	version = QRinput_getVersion(stream);
	assert_equal(version, 40, "Version is %d (40 expected).\n", version);
	testFinish();
	QRinput_free(stream);
	BitStream_free(bstream);
	free(str);
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
	testEnd(cmpBin(correct, bstream));
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
	testEnd(cmpBin(correct, bstream));
	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encodeNumeric_versionup(void)
{
	QRinput *stream;
	BitStream *bstream;
	char *str;
	int version;

	testStart("Encoding numeric stream. (auto-version up test)");
	str = (char *)malloc(1050);
	memset(str, '1', 1050);
	stream = QRinput_new2(0, QR_ECLEVEL_L);
	QRinput_append(stream, QR_MODE_NUM, 1050, (unsigned char *)str);
	bstream = QRinput_mergeBitStream(stream);
	version = QRinput_getVersion(stream);
	assert_equal(version, 14, "Version is %d (14 expected).", version);
	testFinish();
	QRinput_free(stream);
	BitStream_free(bstream);
	free(str);
}

void test_encodeNumericPadded(void)
{
	QRinput *stream;
	char num[9] = "01234567";
	char *correct;
	char *correctHead = "000100000010000000001100010101100110000110000000";
	BitStream *bstream;
	int flag, i;

	testStart("Encoding numeric stream. (8 digits)(padded)");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_NUM, 8, (unsigned char *)num);
	bstream = QRinput_getBitStream(stream);
	correct = (char *)malloc(19 * 8 + 1);
	correct[0] = '\0';
	strcat(correct, correctHead);
	for(i=0; i<13; i++) {
		strcat(correct, (i&1)?"00010001":"11101100");
	}
	flag = cmpBin(correct, bstream);
	testEnd(flag);

	free(correct);
	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encodeNumericPadded2(void)
{
	QRinput *stream;
	char num[8] = "0123456";
	char *correct;
	char *correctHead = "000100000001110000001100010101100101100000000000";
	BitStream *bstream;
	int flag, i;

	testStart("Encoding numeric stream. (7 digits)(padded)");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_NUM, 7, (unsigned char *)num);
	bstream = QRinput_getBitStream(stream);
	correct = (char *)malloc(19 * 8 + 1);
	correct[0] = '\0';
	strcat(correct, correctHead);
	for(i=0; i<13; i++) {
		strcat(correct, (i&1)?"00010001":"11101100");
	}
	flag = cmpBin(correct, bstream);
	testEnd(flag);

	free(correct);
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
	testEnd(cmpBin(correct, bstream));
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
	testEnd(cmpBin(correct, bstream));
	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_encodeTooLong(void)
{
	QRinput *stream;
	unsigned char *data;
	BitStream *bstream;

	data = (unsigned char *)malloc(4297);
	memset(data, 'A', 4297);

	testStart("Encoding long string. (4297 bytes of alphanumeric)");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_AN, 4297, data);
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
	QRinput_append(input, QR_MODE_AN, 11, (unsigned char *)"ABCDEFGHIJK");
	QRinput_append(input, QR_MODE_NUM, 12, (unsigned char *)"123456789012");
	bstream = QRinput_mergeBitStream(input);
	testEndExp(BitStream_size(bstream) == 128);
	QRinput_free(input);
	BitStream_free(bstream);

	testStart("Bit length check of alphabet stream. (23)");
	input = QRinput_new();
	QRinput_append(input, QR_MODE_AN, 23, (unsigned char *)"ABCDEFGHIJK123456789012");
	bstream = QRinput_mergeBitStream(input);
	testEndExp(BitStream_size(bstream) == 140);
	QRinput_free(input);
	BitStream_free(bstream);
}

void test_struct_listop(void)
{
	QRinput_Struct *s;
	QRinput *inputs[5];
	QRinput_InputList *l;
	int i, ret;

	testStart("QRinput_Struct list operation test.");
	s = QRinput_Struct_new();
	QRinput_Struct_setParity(s, 10);
	assert_nonnull(s, "QRinput_Struct_new() failed.");
	assert_equal(s->parity, 10, "QRinput_Struct_setParity() failed.");

	for(i=0; i<5; i++) {
		inputs[i] = QRinput_new();
		QRinput_append(inputs[i], QR_MODE_AN, 5, (unsigned char *)"ABCDE");
		ret = QRinput_Struct_appendInput(s, inputs[i]);
	}
	assert_equal(ret, 5, "QRinput_Struct_appendInput() returns wrong num?");
	assert_equal(s->size, 5, "QRiput_Struct.size counts wrong number.");

	l = s->head;
	i = 0;
	while(l != NULL) {
		assert_equal(l->input, inputs[i], "QRinput_Struct input list order would be wrong?");
		l = l->next;
		i++;
	}

	QRinput_Struct_free(s);
	testFinish();
}

void test_insertStructuredAppendHeader(void)
{
	QRinput *stream;
	char correct[] = "0011000011111010010101000000000101000001";
	BitStream *bstream;
	int ret;

	testStart("Insert a structured-append header");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_8, 1, (unsigned char *)"A");
	ret = QRinput_insertStructuredAppendHeader(stream, 16, 1, 0xa5);
	assert_zero(ret, "QRinput_insertStructuredAppendHeader() returns nonzero.\n");
	bstream = QRinput_mergeBitStream(stream);
	assert_nonnull(bstream->data, "Bstream->data is null.");
	assert_zero(cmpBin(correct, bstream), "bitstream is wrong.");
	testFinish();

	QRinput_free(stream);
	BitStream_free(bstream);
}

void test_insertStructuredAppendHeader_error(void)
{
	QRinput *stream;
	int ret;

	testStart("Insert a structured-append header (errors expected)");
	stream = QRinput_new();
	QRinput_append(stream, QR_MODE_8, 1, (unsigned char *)"A");
	ret = QRinput_insertStructuredAppendHeader(stream, 17, 1, 0xa5);
	assert_equal(-1, ret, "QRinput_insertStructuredAppendHeader() returns 0.");
	assert_equal(EINVAL, errno, "errno is not set correctly (%d returned).", errno);
	ret = QRinput_insertStructuredAppendHeader(stream, 16, 17, 0xa5);
	assert_equal(-1, ret, "QRinput_insertStructuredAppendHeader() returns 0.");
	assert_equal(EINVAL, errno, "errno is not set correctly (%d returned).", errno);
	ret = QRinput_insertStructuredAppendHeader(stream, 16, 0, 0xa5);
	assert_equal(-1, ret, "QRinput_insertStructuredAppendHeader() returns 0.");
	assert_equal(EINVAL, errno, "errno is not set correctly (%d returned).", errno);
	testFinish();

	QRinput_free(stream);
}

void test_struct_insertStructuredAppendHeaders(void)
{
	QRinput *input;
	QRinput_Struct *s;
	QRinput_InputList *p;
	int i;

	testStart("Insert structured-append headers to a QRinput_Struct.");
	s = QRinput_Struct_new();
	for(i=0; i<10; i++) {
		input = QRinput_new();
		QRinput_append(input, QR_MODE_8, 1, (unsigned char *)"A");
		QRinput_Struct_appendInput(s, input);
	}
	QRinput_Struct_insertStructuredAppendHeaders(s);
	p = s->head;
	i = 1;
	while(p != NULL) {
		assert_equal(p->input->head->mode, QR_MODE_STRUCTURE, "a structured-append header is not inserted.");
		assert_equal(p->input->head->data[0], 10, "size of the structured-header is wrong: #%d, %d should be %d\n", i, p->input->head->data[0], 10);
		assert_equal(p->input->head->data[1], i, "index of the structured-header is wrong: #%d, %d should be %d\n", i, p->input->head->data[1], i);
		assert_equal(p->input->head->data[2], 0, "parity of the structured-header is wrong: #%d\n", i);
		p = p->next;
		i++;
	}
	testFinish();
	QRinput_Struct_free(s);
}

static int check_lengthOfCode(QRencodeMode mode, char *data, int size, int version)
{
	QRinput *input;
	BitStream *b;
	int bits;
	int bytes;

	input = QRinput_new();
	QRinput_setVersion(input, version);
	QRinput_append(input, mode, size, (unsigned char *)data);
	b = QRinput_mergeBitStream(input);
	bits = BitStream_size(b);
	bytes = QRinput_lengthOfCode(mode, version, bits);
	QRinput_free(input);
	BitStream_free(b);

	return bytes;
}

void test_lengthOfCode_num(void)
{
	int i, bytes;
	char *data;

	data = (char *)malloc(8000);
	for(i=0; i<8000; i++) {
		data[i] = '0' + i % 10;
	}

	testStart("Checking length of code (numeric)");
	for(i=1; i<=9; i++) {
		bytes = check_lengthOfCode(QR_MODE_NUM, data, i, 1);
		assert_equal(i, bytes, "lengthOfCode failed. (QR_MODE_NUM, version:1, size:%d)\n", i);
	}
	for(i=1023; i<=1025; i++) {
		bytes = check_lengthOfCode(QR_MODE_NUM, data, i, 1);
		assert_equal(1023, bytes, "lengthOfCode failed. (QR_MODE_NUM, version:1, size:%d)\n", i);
	}
	testFinish();
	free(data);
}

void test_lengthOfCode_kanji(void)
{
	int i, bytes;
	char str[4]= {0x93, 0x5f,0xe4, 0xaa};

	testStart("Checking length of code (kanji)");
	for(i=2; i<=4; i+=2) {
		bytes = check_lengthOfCode(QR_MODE_KANJI, str, i, 1);
		assert_equal(i, bytes, "lengthOfCode failed. (QR_MODE_KANJI, version:1, size:%d)\n", i);
	}
	testFinish();
}

void test_struct_split_example(void)
{
	QRinput *input;
	QRinput_Struct *s;
	QRinput_InputList *e;
	QRinput_List *l;
	const char *str[4] = { "an example ", "of four Str", "uctured Appe", "nd symbols,"};
	int i;
	BitStream *bstream;

	testStart("Testing the example of structured-append symbols");
	s = QRinput_Struct_new();
	for(i=0; i<4; i++) {
		input = QRinput_new2(1, QR_ECLEVEL_M);
		QRinput_append(input, QR_MODE_8, strlen(str[i]), (unsigned char *)str[i]);
		QRinput_Struct_appendInput(s, input);
	}
	QRinput_Struct_insertStructuredAppendHeaders(s);
	e = s->head;
	i = 0;
	while(e != NULL) {
		bstream = QRinput_mergeBitStream(e->input);
		BitStream_free(bstream);
		l = e->input->head->next;
		assert_equal(l->mode, QR_MODE_8, "#%d: wrong mode (%d).\n", i, l->mode);
		assert_equal(e->input->level, QR_ECLEVEL_M, "#%d: wrong level (%d).\n", i, e->input->level);

		e = e->next;
		i++;
	}
	testFinish();
	QRinput_Struct_free(s);
}

void test_struct_split_tooLarge(void)
{
	QRinput *input;
	QRinput_Struct *s;
	char *str;
	int errsv;

	testStart("Testing structured-append symbols. (too large data)");
	str = (char *)malloc(128);
	memset(str, 'a', 128);
	input = QRinput_new2(1, QR_ECLEVEL_H);
	QRinput_append(input, QR_MODE_8, 128, (unsigned char *)str);
	s = QRinput_splitQRinputToStruct(input);
	errsv = errno;
	assert_null(s, "returns non-null.");
	assert_equal(errsv, ERANGE, "did not return ERANGE.");
	testFinish();
	if(s != NULL) QRinput_Struct_free(s);
	QRinput_free(input);
	free(str);
}

void test_struct_split_invalidVersion(void)
{
	QRinput *input;
	QRinput_Struct *s;
	char *str;
	int errsv;

	testStart("Testing structured-append symbols. (invalid version 0)");
	str = (char *)malloc(128);
	memset(str, 'a', 128);
	input = QRinput_new2(0, QR_ECLEVEL_H);
	QRinput_append(input, QR_MODE_8, 128, (unsigned char *)str);
	s = QRinput_splitQRinputToStruct(input);
	errsv = errno;
	assert_null(s, "returns non-null.");
	assert_equal(errsv, ERANGE, "did not return ERANGE.");
	testFinish();
	if(s != NULL) QRinput_Struct_free(s);
	QRinput_free(input);
	free(str);
}

void test_splitentry(void)
{
	QRinput *i1, *i2;
	QRinput_List *e;
	const char *str = "abcdefghij";
	int size1, size2, i;
	unsigned char *d1, *d2;

	testStart("Testing QRinput_splitEntry. (next == NULL)");
	i1 = QRinput_new();
	QRinput_append(i1, QR_MODE_8, strlen(str), (unsigned char *)str);

	i2 = QRinput_dup(i1);
	e = i2->head;

	e = i2->head;
	QRinput_splitEntry(e, 4);

	size1 = size2 = 0;
	e = i1->head;
	while(e != NULL) {
		size1 += e->size;
		e = e->next;
	}
	e = i2->head;
	while(e != NULL) {
		size2 += e->size;
		e = e->next;
	}

	d1 = (unsigned char *)malloc(size1);
	e = i1->head;
	i = 0;
	while(e != NULL) {
		memcpy(&d1[i], e->data, e->size);
		i += e->size;
		e = e->next;
	}
	d2 = (unsigned char *)malloc(size2);
	e = i2->head;
	i = 0;
	while(e != NULL) {
		memcpy(&d2[i], e->data, e->size);
		i += e->size;
		e = e->next;
	}

	assert_equal(size1, size2, "sizes are different. (%d:%d)\n", size1, size2);
	assert_equal(i2->head->size, 4, "split failed (first half)");
	assert_equal(i2->head->next->size, 6, "split failed(second half)");
	assert_zero(memcmp(d1, d2, size1), "strings are different.");
	QRinput_free(i1);
	QRinput_free(i2);
	free(d1);
	free(d2);

	testFinish();
}

void test_splitentry2(void)
{
	QRinput *i1, *i2;
	QRinput_List *e;
	const char *str = "abcdefghij";
	int size1, size2, i;
	unsigned char *d1, *d2;

	testStart("Testing QRinput_splitEntry. (next != NULL)");
	i1 = QRinput_new();
	QRinput_append(i1, QR_MODE_8, strlen(str), (unsigned char *)str);
	QRinput_append(i1, QR_MODE_8, strlen(str), (unsigned char *)str);

	i2 = QRinput_dup(i1);
	e = i2->head;

	e = i2->head;
	QRinput_splitEntry(e, 4);

	size1 = size2 = 0;
	e = i1->head;
	while(e != NULL) {
		size1 += e->size;
		e = e->next;
	}
	e = i2->head;
	while(e != NULL) {
		size2 += e->size;
		e = e->next;
	}

	d1 = (unsigned char *)malloc(size1);
	e = i1->head;
	i = 0;
	while(e != NULL) {
		memcpy(&d1[i], e->data, e->size);
		i += e->size;
		e = e->next;
	}
	d2 = (unsigned char *)malloc(size2);
	e = i2->head;
	i = 0;
	while(e != NULL) {
		memcpy(&d2[i], e->data, e->size);
		i += e->size;
		e = e->next;
	}

	assert_equal(size1, size2, "sizes are different. (%d:%d)\n", size1, size2);
	assert_equal(i2->head->size, 4, "split failed (first half)");
	assert_equal(i2->head->next->size, 6, "split failed(second half)");
	assert_zero(memcmp(d1, d2, size1), "strings are different.");
	QRinput_free(i1);
	QRinput_free(i2);
	free(d1);
	free(d2);

	testFinish();
}

void test_splitentry3(void)
{
	QRinput *input;
	QRinput_Struct *s;
	QRinput_List *e00, *e01, *e10, *e11;
	QRinput_InputList *list;
	const char *str = "abcdefghijklmno";

	testStart("Testing QRinput_splitEntry. (does not split an entry)");
	/* version 1 symbol contains 152 bit (19 byte) data.
	 * 20 bits for a structured-append header, so 132 bits can be used.
	 * 15 bytes of 8-bit data is suitable for the symbol.
	 * (mode(4) + length(8) + data(120) == 132.)
	 */
	input = QRinput_new2(1, QR_ECLEVEL_L);
	QRinput_append(input, QR_MODE_8, strlen(str), (unsigned char *)str);
	QRinput_append(input, QR_MODE_8, strlen(str), (unsigned char *)str);
	s = QRinput_splitQRinputToStruct(input);
	list = s->head;
	e00 = list->input->head;
	e01 = e00->next;
	list = list->next;
	e10 = list->input->head;
	e11 = e00->next;
	
	assert_equal(e00->mode, QR_MODE_STRUCTURE, "Structure header is missing?");
	assert_equal(e01->mode, QR_MODE_8, "no data?!");
	assert_null(e01->next, "Input list is not terminated!\n");
	assert_equal(e10->mode, QR_MODE_STRUCTURE, "Structure header is missing?");
	assert_equal(e11->mode, QR_MODE_8, "no data?!");
	assert_null(e11->next, "Input list is not terminated!\n");

	QRinput_free(input);
	QRinput_Struct_free(s);
	testFinish();
}

void test_parity(void)
{
	QRinput *input;
	QRinput_Struct *s;
	const char *text = "an example of four Structured Append symbols,";
	const char *str[4] = {
		"an example ",
		"of four Str",
		"uctured Appe",
		"nd symbols,"};
	unsigned char p1, p2;
	int i, len;

	testStart("Testing parity calc.");
	s = QRinput_Struct_new();
	for(i=0; i<4; i++) {
		input = QRinput_new2(1, QR_ECLEVEL_M);
		QRinput_append(input, QR_MODE_8, strlen(str[i]), (unsigned char *)str[i]);
		QRinput_Struct_appendInput(s, input);
	}
	QRinput_Struct_insertStructuredAppendHeaders(s);
	p1 = s->parity;

	p2 = 0;
	len = strlen(text);
	for(i=0; i<len; i++) {
		p2 ^= text[i];
	}
	assert_equal(p1, p2, "Parity numbers didn't match. (%02x should be %02x).\n", p1, p2);
	testFinish();
	QRinput_Struct_free(s);
}

void test_parity2(void)
{
	QRinput *input;
	QRinput_Struct *s;
	const char *text = "an example of four Structured Append symbols,";
	unsigned char p1, p2;
	int i, len;

	testStart("Testing parity calc.(split)");
	input = QRinput_new2(1, QR_ECLEVEL_L);
	QRinput_append(input, QR_MODE_8, strlen(text), (unsigned char *)text);
	s = QRinput_splitQRinputToStruct(input);
	p1 = s->parity;

	p2 = 0;
	len = strlen(text);
	for(i=0; i<len; i++) {
		p2 ^= text[i];
	}
	assert_equal(p1, p2, "Parity numbers didn't match. (%02x should be %02x).\n", p1, p2);
	testFinish();
	QRinput_free(input);
	QRinput_Struct_free(s);
}

int main(int argc, char **argv)
{
	test_encodeNumeric();
	test_encodeNumeric2();
	test_encodeNumeric3();
	test_encodeNumeric_versionup();
	test_encode8();
	test_encode8_versionup();
	test_encodeTooLong();
	test_encodeAn();
	test_encodeAn2();
	test_encodeKanji();
	test_encodeNumericPadded();
	test_encodeNumericPadded2();
	test_encodeAnNum();
	test_struct_listop();
	test_insertStructuredAppendHeader();
	test_insertStructuredAppendHeader_error();
	test_struct_insertStructuredAppendHeaders();
	test_lengthOfCode_num();
	test_splitentry();
	test_splitentry2();
	test_splitentry3();
	test_struct_split_example();
	test_struct_split_tooLarge();
	test_struct_split_invalidVersion();
	test_parity();
	test_parity2();

	report();

	return 0;
}
