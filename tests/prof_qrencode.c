#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "../qrencode.h"

struct timeval tv;
void timerStart(const char *str)
{
	printf("%s: START\n", str);
	gettimeofday(&tv, NULL);
}

void timerStop(void)
{
	struct timeval tc;

	gettimeofday(&tc, NULL);
	printf("STOP: %ld msec\n", (tc.tv_sec - tv.tv_sec) * 1000
			+ (tc.tv_usec - tv.tv_usec) / 1000);
}

void prof_ver1to10(void)
{
	QRcode *code;
	int i;
	int version;
	static char *data = "This is test.";

	timerStart("Version 1 - 10");
	for(i=0; i<500; i++) {
		for(version = 0; version < 11; version++) {
			code = QRcode_encodeString(data, version, QR_ECLEVEL_L, QR_MODE_8);
			QRcode_free(code);
		}
	}
	timerStop();
}

int main()
{
	prof_ver1to10();

	return 0;
}
