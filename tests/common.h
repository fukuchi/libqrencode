/*
 * common part of test units.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdlib.h>
#include "../qrencode.h"
#include "../bitstream.h"

#define CHECK(_str_) (printf("_____%s\n",_str_))
#define RESULT(_args_...) (printf(".....") + printf(_args_))

#define testStart(__arg__) (testStartReal(__FUNCTION__, __arg__))
#define testEndExp(__arg__) (testEnd(!(__arg__)))

static int tests = 0;
static int failed = 0;
static const char *testName = NULL;
static const char *testFunc = NULL;

void testStartReal(const char *func, const char *name)
{
	tests++;
	testName = name;
	testFunc = func;
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

void report()
{
	printf("Total %d tests, %d fails.\n", tests, failed);
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
#endif /* __COMMON_H__ */
