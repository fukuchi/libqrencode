/*
 * common part of test units.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include "../qrencode.h"

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

#endif /* __COMMON_H__ */
