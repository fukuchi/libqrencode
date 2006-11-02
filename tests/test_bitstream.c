#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../bitstream.h"

void test_1bitStream(void)
{
	unsigned int data;
	BitStream *bstream;
	int i, flag;

	testStart("1 bit stream");
	bstream = BitStream_new(128);
	data = 1;
	for(i=0; i<128; i++) {
		BitStream_append(bstream,  1, (unsigned char *)&data);
	}

	flag = 0;
	for(i=0; i<16; i++) {
		printf("%02x", bstream->data[i]);
		if(bstream->data[i] != 0xff)
			flag++;
	}
	printf("\n");

	testEnd(flag);
}

void test_8nbitsStream(void)
{
	unsigned int data;
	BitStream *bstream;
	unsigned char correct[7];

	testStart("8n bit stream");
	bstream = BitStream_new(56);
	data = 0x8a;
	correct[0] = 0x8a;
	BitStream_append(bstream,  8, (unsigned char *)&data);

	data = 0xffff;
	correct[1] = 0xff;
	correct[2] = 0xff;
	BitStream_append(bstream, 16, (unsigned char *)&data);

	data = 0x12345678;
	correct[3] = 0x78;
	correct[4] = 0x56;
	correct[5] = 0x34;
	correct[6] = 0x12;
	BitStream_append(bstream, 32, (unsigned char *)&data);

	testEnd(memcmp(correct, bstream->data, 7));
}

void test_varStream(void)
{
	unsigned int data;
	BitStream *bstream;
	int flag;
	unsigned char correct[7];
	int i;

	testStart("Various bit stream");
	bstream = BitStream_new(54);
	data = 0x7f;
	correct[0] = 0x7f;
	BitStream_append(bstream, 7, (unsigned char *)&data);

	data = 0x0555;
	correct[0] |= 0x80;
	correct[1] = (0x55>>1) | (0x55<<7);
	correct[2] = 0x05>>1;
	BitStream_append(bstream, 14, (unsigned char *)&data);

	data = 0x12345678;
	correct[2] |= 0x78 << 5;
	correct[3] = (0x78 >> 3) | (0x56 << 5);
	correct[4] = (0x56 >> 3) | (0x34 << 5);
	correct[5] = (0x34 >> 3) | (0x12 << 5);
	correct[6] = 0x12 >> 3;
	BitStream_append(bstream, 29, (unsigned char *)&data);

	data = 0xff;
	correct[6] |= (0xff & 0x0f) << 2;
	BitStream_append(bstream, 4, (unsigned char *)&data);

	for(i=0; i<7; i++) {
		printf("%02x", correct[i]);
	}
	printf("\n");
	for(i=0; i<7; i++) {
		printf("%02x", bstream->data[i]);
	}
	printf("\n");

	flag = memcmp(correct, bstream->data, 6);
	flag |= (bstream->data[6] & 0x3f) != (correct[6] & 0x3f);
	testEnd(flag);
}

int main(int argc, char **argv)
{
    test_8nbitsStream();
	test_varStream();
	test_1bitStream();

	report();

    return 0;
}
