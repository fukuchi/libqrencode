#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../mask.h"
#include "../qrspec.h"

char dot[2] = {'_', '#'};
static char *maskPatterns[8] = {
	/* (i + j) mod 2 = 0 */
	"#_#_#_"
	"_#_#_#"
	"#_#_#_"
	"_#_#_#"
	"#_#_#_"
	"_#_#_#",
	/* i mod 2 = 0 */
	"######"
	"______"
	"######"
	"______"
	"######"
	"______",
	/* j mod 3 = 0 */
	"#__#__"
	"#__#__"
	"#__#__"
	"#__#__"
	"#__#__"
	"#__#__",
	/* (i + j) mod 3 = 0 */
	"#__#__"
	"__#__#"
	"_#__#_"
	"#__#__"
	"__#__#"
	"_#__#_",
	/* ((i div 2) + (j div 3)) mod 2 = 0 */
	"###___"
	"###___"
	"___###"
	"___###"
	"###___"
	"###___",
	/* (ij) mod 2 + (ij) mod 3 = 0 */
	"######"
	"#_____"
	"#__#__"
	"#_#_#_"
	"#__#__"
	"#_____",
	/* ((ij) mod 2 + (ij) mod 3) mod 2 = 0 */
	"######"
	"###___"
	"##_##_"
	"#_#_#_"
	"#_##_#"
	"#___##",
	/* ((ij) mod 3 + (i+j) mod 2) mod 2 = 0 */
	"#_#_#_"
	"___###"
	"#___##"
	"_#_#_#"
	"###___"
	"_###__"
};

void print_mask(int mask)
{
	const int w = 6;
	unsigned char frame[w * w], *masked, *p;
	int x, y;

	memset(frame, 0, w * w);
	masked = Mask_makeMaskedFrame(w, frame, mask);
	p = masked;
	for(y=0; y<w; y++) {
		for(x=0; x<w; x++) {
			putchar(dot[*p&1]);
			p++;
		}
		printf("\n");
	}
	printf("\n");

	free(masked);
}

void print_masks(void)
{
	int i;

	for(i=0; i<8; i++) {
		print_mask(i);
	}
}

int test_mask(int mask)
{
	const int w = 6;
	unsigned char frame[w * w], *masked, *p;
	char *q;
	int x, y;
	int err = 0;

	memset(frame, 0, w * w);
	masked = Mask_makeMaskedFrame(w, frame, mask);
	p = masked;
	q = maskPatterns[mask];
	for(y=0; y<w; y++) {
		for(x=0; x<w; x++) {
			if(dot[*p&1] != *q) {
				err++;
			}
			p++;
			q++;
		}
	}

	free(masked);
	return err;
}

void test_masks(void)
{
	int i;

	testStart("Mask pattern checks");
	for(i=0; i<8; i++) {
		assert_zero(test_mask(i), "Mask pattern %d incorrect.\n", i);
	}
	testFinish();
}

#define N1 (3)
#define N2 (3)
#define N3 (40)
#define N4 (10)

void test_eval(void)
{
	unsigned char *frame;
	int w = 6;
	int demerit;

	frame = (unsigned char *)malloc(w * w);

	testStart("Test mask evaluation (all white)");
	memset(frame, 0, w * w);
	demerit = Mask_evaluateSymbol(w, frame);
	testEndExp(demerit == ((N1 + 1)*w*2 + N2 * (w - 1) * (w - 1)));

	testStart("Test mask evaluation (all black)");
	memset(frame, 1, w * w);
	demerit = Mask_evaluateSymbol(w, frame);
	testEndExp(demerit == ((N1 + 1)*w*2 + N2 * (w - 1) * (w - 1)));

	free(frame);
}

/* .#.#.#.#.#
 * #.#.#.#.#.
 * ..##..##..
 * ##..##..##
 * ...###...#
 * ###...###.
 * ....####..
 * ####....##
 * .....#####
 * #####.....
 */
void test_eval2(void)
{
	unsigned char *frame;
	int w = 10;
	int demerit;
	int x;

	frame = (unsigned char *)malloc(w * w);

	testStart("Test mask evaluation (run length penalty check)");
	for(x=0; x<w; x++) {
		frame[      x] = x & 1;
		frame[w   + x] = (x & 1) ^ 1;
		frame[w*2 + x] = (x / 2) & 1;
		frame[w*3 + x] = ((x / 2) & 1) ^ 1;
		frame[w*4 + x] = (x / 3) & 1;
		frame[w*5 + x] = ((x / 3) & 1) ^ 1;
		frame[w*6 + x] = (x / 4) & 1;
		frame[w*7 + x] = ((x / 4) & 1) ^ 1;
		frame[w*8 + x] = (x / 5) & 1;
		frame[w*9 + x] = ((x / 5) & 1) ^ 1;
	}
	demerit = Mask_evaluateSymbol(w, frame);
	testEndExp(demerit == N1 * 4 + N2 * 4);

	free(frame);
}

void test_eval3(void)
{
	unsigned char *frame;
	int w = 15;
	int demerit;
	int x, y;
	static unsigned char pattern[7][15] = {
		{0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0}, // N3x1
		{1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1}, // N3x1
		{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1}, // N3x1
		{1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0}, // 0
		{1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1}, // N3x2
		{1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0}, // N3 + (N1+1)
		{1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1}  // (N1+1)
	};

	frame = (unsigned char *)malloc(w * w);

	testStart("Test mask evaluation (1:1:3:1:1 check)");
	for(y=0; y<5; y++) {
		for(x=0; x<w; x++) {
			frame[w*y*2     + x] = pattern[y][x];
			frame[w*(y*2+1) + x] = pattern[y][x]^1;
		}
	}
	for(x=0; x<w; x++) {
		frame[w*10 + x] = x & 1;
	}
	for(y=5; y<7; y++) {
		for(x=0; x<w; x++) {
			frame[w*(y*2+1) + x] = pattern[y][x];
			frame[w*(y*2+2) + x] = pattern[y][x]^1;
		}
	}
	demerit = Mask_evaluateSymbol(w, frame);
	testEndExp(demerit == N3 * 6 + (N1 + 1) * 4);

	free(frame);
}

int main(void)
{
	//print_masks();
	test_masks();
	test_eval();
	test_eval2();
	test_eval3();

	report();

	return 0;
}
