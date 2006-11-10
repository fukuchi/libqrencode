#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../qrspec.h"
#include "../qrtest.h"

void test_eccTable(void)
{
	int i, j;
	int ecc;
	int data;
	int err = 0;
	int *bl;

	testStart("Checking ECC table.");
	for(i=1; i<QRSPEC_VERSION_MAX; i++) {
		for(j=0; j<4; j++) {
			bl = QRspec_getEccBlockNum(i, j);
			data = bl[0] * bl[1] + bl[3] * bl[4];
			ecc  = bl[0] * bl[2] + bl[3] * bl[5];
			if(qrspecCapacity[i].words != data + ecc) {
				printf("Error in version %d, level %d: invalid size\n", i, j);
				printf("%d %d %d %d %d %d\n", bl[0], bl[1], bl[2], bl[3], bl[4], bl[5]);
				err++;
			}
			if(qrspecCapacity[i].ec[j] != ecc) {
				printf("Error in version %d, level %d: invalid data\n", i, j);
				printf("%d %d %d %d %d %d\n", bl[0], bl[1], bl[2], bl[3], bl[4], bl[5]);
				err++;
			}
		}
	}
	testEnd(err);
}

int main(int argc, char **argv)
{
	test_eccTable();

	report();

	return 0;
}
