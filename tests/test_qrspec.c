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
			bl = QRspec_getEccSpec(i, j);
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
			bl = QRspec_getEccSpec(i, j);
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
		bl = QRspec_getEccSpec(correct[i][0], correct[i][1]);
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

void test_alignment1(void)
{
	QRspec_Alignment *al;
	int i;
	int err = 0;
	int rbpos;

	testStart("Checking alignment pattern table(1)");
	rbpos = 14;
	for(i=1; i<=QRSPEC_VERSION_MAX; i++) {
		al = QRspec_getAlignmentPattern(i);
		if(i == 1) {
			if(al != NULL) {
				printf("Error in version %d.\n", i);
				err++;
			}
		} else if(i < 7) {
			if(al->n != 1) {
				printf("Error in version %d.\n", i);
				err++;
			}
			if(al->pos[al->n*2-1] != rbpos) {
				printf("Error in version %d.\n", i);
				err++;
			}
		} else if(i < 14) {
			if(al->n != 6) {
				printf("Error in version %d.(%d)\n", i, al->n);
				err++;
			}
			if(al->pos[al->n*2-1] != rbpos) {
				printf("Error in version %d.\n", i);
				err++;
			}
		} else if(i < 21) {
			if(al->n != 13) {
				printf("Error in version %d.(%d)\n", i, al->n);
				err++;
			}
			if(al->pos[al->n*2-1] != rbpos) {
				printf("Error in version %d.\n", i);
				err++;
			}
		} else if(i < 28) {
			if(al->n != 22) {
				printf("Error in version %d.(%d)\n", i, al->n);
				err++;
			}
			if(al->pos[al->n*2-1] != rbpos) {
				printf("Error in version %d.\n", i);
				err++;
			}
		} else if(i < 35) {
			if(al->n != 33) {
				printf("Error in version %d.(%d)\n", i, al->n);
				err++;
			}
			if(al->pos[al->n*2-1] != rbpos) {
				printf("Error in version %d.\n", i);
				err++;
			}
		} else {
			if(al->n != 46) {
				printf("Error in version %d.(%d)\n", i, al->n);
				err++;
			}
			if(al->pos[al->n*2-1] != rbpos) {
				printf("Error in version %d.\n", i);
				err++;
			}
		}
		QRspec_freeAlignment(al);
		rbpos += 4;
	}
	testEnd(err);
}

void test_verpat(void)
{
	int version;
	unsigned int pattern;
	int err = 0;
	unsigned int data;
	unsigned int code;
	int i, c;
	unsigned int mask;

	for(version=7; version <= QRSPEC_VERSION_MAX; version++) {
		pattern = QRspec_getVersionPattern(version);
		if((pattern >> 12) != version) {
			printf("Error in version %d.\n", version);
			err++;
			continue;
		}
		mask = 0x40;
		for(i=0; mask != 0; i++) {
			if(version & mask) break;
			mask = mask >> 1;
		}
		c = 6 - i;
		data = version << 12;
		code = 0x1f25 << c;
		mask = 0x40000 >> (6 - c);
		for(i=0; i<=c; i++) {
			if(mask & data) {
				data ^= code;
			}
			code = code >> 1;
			mask = mask >> 1;
		}
		data = (version << 12) | (data & 0xfff);
		if(data != pattern) {
			printf("Error in version %d\n", version);
			err++;
		}
	}
}

int main(int argc, char **argv)
{
	test_eccTable();
	test_eccTable2();
//	print_eccTable();
	test_alignment1();
	test_verpat();

	report();

	return 0;
}
