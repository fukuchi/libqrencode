#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../mmask.h"
#include "../mqrspec.h"

void print_mask(int mask)
{
	const int w = 8;
	unsigned char frame[w * w], *masked, *p;
	int x, y;

	memset(frame, 0, w * w);
	masked = MMask_makeMaskedFrame(w, frame, mask);
	p = masked;
	for(y=0; y<w; y++) {
		for(x=0; x<w; x++) {
			if(*p & 1) {
				printf("#");
			} else {
				printf("_");
			}
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

	for(i=0; i<4; i++) {
		print_mask(i);
	}
}

int main(void)
{
	print_masks();

	report();

	return 0;
}
