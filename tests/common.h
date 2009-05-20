/*
 * common part of test units.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdlib.h>
#include "../qrencode.h"
#include "../qrinput.h"
#include "../bitstream.h"
#include "../qrencode_inner.h"

#define testStart(__arg__) (testStartReal(__func__, __arg__))
#define testEndExp(__arg__) (testEnd(!(__arg__)))

static int tests = 0;
static int failed = 0;
static int assertionFailed = 0;
static int assertionNum = 0;
static const char *testName = NULL;
static const char *testFunc = NULL;
char levelChar[4] = {'L', 'M', 'Q', 'H'};
const char *modeStr[5] = {"nm", "an", "8", "kj", "st"};

void printQRinput(QRinput *input)
{
	QRinput_List *list;
	unsigned char *p;
	int i;

	list = input->head;
	while(list != NULL) {
		p = list->data;
		for(i=0; i<list->size; i++) {
			printf("0x%02x,", list->data[i]);
		}
		list = list->next;
	}
	printf("\n");
}

void printQRinputInfo(QRinput *input)
{
	QRinput_List *list;
	BitStream *b;
	int i;

	printf("QRinput info:\n");
	printf(" version: %d\n", input->version);
	printf(" level  : %c\n", levelChar[input->level]);
	list = input->head;
	i = 0;
	while(list != NULL) {
		i++;
		list = list->next;
	}
	printf("  chunks: %d\n", i);
	b = QRinput_mergeBitStream(input);
	if(b != NULL) {
		printf("  bitstream-size: %d\n", BitStream_size(b));
		BitStream_free(b);
	}

	list = input->head;
	i = 0;
	while(list != NULL) {
		printf("\t#%d: mode = %s, size = %d\n", i, modeStr[list->mode], list->size);
		i++;
		list = list->next;
	}
}

void testStartReal(const char *func, const char *name)
{
	tests++;
	testName = name;
	testFunc = func;
	assertionFailed = 0;
	assertionNum = 0;
	printf("_____%d: %s: %s...\n", tests, func, name);
}

void testEnd(int result)
{
	printf(".....%d: %s: %s, ", tests, testFunc, testName);
	if(result) {
		puts("FAILED.");
		failed++;
	} else {
		puts("PASSED.");
	}
}

#define assert_exp(__exp__, ...) \
{assertionNum++;if(!(__exp__)) {assertionFailed++; printf(__VA_ARGS__);}}

#define assert_zero(__exp__, ...) assert_exp((__exp__) == 0, __VA_ARGS__)
#define assert_nonzero(__exp__, ...) assert_exp((__exp__) != 0, __VA_ARGS__)
#define assert_null(__ptr__, ...) assert_exp((__ptr__) == NULL, __VA_ARGS__)
#define assert_nonnull(__ptr__, ...) assert_exp((__ptr__) != NULL, __VA_ARGS__)
#define assert_equal(__e1__, __e2__, ...) assert_exp((__e1__) == (__e2__), __VA_ARGS__)
#define assert_notequal(__e1__, __e2__, ...) assert_exp((__e1__) != (__e2__), __VA_ARGS__)
#define assert_nothing(__exp__, ...) {printf(__VA_ARGS__); __exp__;}

void testFinish(void)
{
	printf(".....%d: %s: %s, ", tests, testFunc, testName);
	if(assertionFailed) {
		printf("FAILED. (%d assertions failed.)\n", assertionFailed);
		failed++;
	} else {
		printf("PASSED. (%d assertions passed.)\n", assertionNum);
	}
}

void report()
{
	printf("Total %d tests, %d fails.\n", tests, failed);
	if(failed) exit(-1);
}

int ncmpBin(char *correct, BitStream *bstream, int len)
{
	int i, bit;

	if(len != BitStream_size(bstream)) {
		printf("Length is not match: %d, %d expected.\n", BitStream_size(bstream), len);
		return -1;
	}

	for(i=0; i<len; i++) {
		bit = (correct[i] == '1')?1:0;
		if(bstream->data[i] != bit) return -1;
	}

	return 0;
}

int cmpBin(char *correct, BitStream *bstream)
{
	int len;

	len = strlen(correct);
	return ncmpBin(correct, bstream, len);
}

char *sprintfBin(int size, unsigned char *data)
{
	int i, j;
	unsigned char mask;
	int b, r;
	char *str, *p;

	str = (char *)malloc(size + 1);
	p = str;
	b = size / 8;
	for(i=0; i<b; i++) {
		mask = 0x80;
		for(j=0; j<8; j++) {
			if(data[i] & mask) {
				*p = '1';
			} else {
				*p = '0';
			}
			p++;
			mask = mask >> 1;
		}
	}
	r = size - b * 8;
	if(r) {
		mask = 1 << (r - 1);
		for(i=0; i<r; i++) {
			if(data[b] & mask) {
				*p = '1';
			} else {
				*p = '0';
			}
			p++;
			mask = mask >> 1;
		}
	}
	*p = '\0';

	return str;
}

static char qrModeChar[4] = {'n', 'a', '8', 'k'};
void printQrinput(QRinput *input)
{
	QRinput_List *list;

	list = input->head;
	while(list != NULL) {
		printf("%c(%d)\n", qrModeChar[list->mode], list->size);
		list = list->next;
	}
}
#endif /* __COMMON_H__ */
