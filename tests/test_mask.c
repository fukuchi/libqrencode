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

int main(void)
{
	//print_masks();
	test_masks();

	report();

	return 0;
}
