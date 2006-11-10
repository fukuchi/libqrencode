#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../qrspec.h"
#include "../qrtest.h"

void print_eccTable(void)
{
	int i, j;
	int ecc;
	int data;
	int *bl;

	for(i=1; i<=QRSPEC_VERSION_MAX; i++) {
		printf("Version %2d\n", i);
		for(j=0; j<4; j++) {
			bl = QRspec_getEccBlockNum(i, j);
			data = bl[0] * bl[1] + bl[3] * bl[4];
			ecc  = bl[0] * bl[2] + bl[3] * bl[5];
			printf("%3d\t", ecc);
			printf("%2d\t", bl[0]);
			printf("(%3d, %3d)\n", bl[1]+bl[2], bl[1]);
			if(bl[3]>0) {
				printf("\t%2d\t", bl[3]);
				printf("(%3d, %3d)\n", bl[4]+bl[5], bl[4]);
			}
			free(bl);
		}
	}
}

void test_eccTable(void)
{
	int i, j;
	int ecc;
	int data;
	int err = 0;
	int *bl;

	testStart("Checking ECC table.");
	for(i=1; i<=QRSPEC_VERSION_MAX; i++) {
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
			free(bl);
		}
	}
	testEnd(err);
}

void test_eccTable2(void)
{
	int i;
	int idx;
	int err;
	int terr = 0;
	int *bl;

	const int correct[7][6] = {
		{ 8,  1, 0,  2, 60, 38},
		{ 8,  1, 1,  2, 61, 39},
		{24,  2, 0, 11, 54, 24},
		{24,  2, 1, 16, 55, 25},
		{32,  0, 0, 17, 145, 115},
		{40,  3, 0, 20, 45, 15},
		{40,  3, 1, 61, 46, 16},
	};

	testStart("Checking ECC table(2)");
	for(i=0; i<7; i++) {
		err = 0;
		bl = QRspec_getEccBlockNum(correct[i][0], correct[i][1]);
		idx = correct[i][2] * 3;
		if(bl[idx] != correct[i][3]) err++;
		if(bl[idx+1] + bl[idx+2] != correct[i][4]) err++;
		if(bl[idx+1] != correct[i][5]) err++;
		if(err) {
			printf("Error in version %d, level %d: invalid data\n",
					correct[i][0], correct[i][1]);
			terr++;
		}
		free(bl);
	}
	testEnd(terr);
}

int main(int argc, char **argv)
{
	test_eccTable();
	test_eccTable2();
//	print_eccTable();

	report();

	return 0;
}
