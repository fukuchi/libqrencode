#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../bitstream.h"

void test_num(void)
{
	BitStream *bstream;
	unsigned int data = 0x13579bdf;
	char correct[] = "0010011010101111001101111011111";

	testStart("New from num");
	bstream = BitStream_new();
	BitStream_appendNum(bstream, 31, data);
	testEnd(strncmp(correct, bstream->data, 31));

	BitStream_free(bstream);
}

void test_bytes(void)
{
	BitStream *bstream;
	unsigned char data[1] = {0x3a};
	char correct[] = "00111010";

	testStart("New from bytes");
	bstream = BitStream_new();
	BitStream_appendBytes(bstream, 1, data);
	testEnd(strncmp(correct, bstream->data, 8));
	BitStream_free(bstream);
}

void test_appendBytes(void)
{
	BitStream *bstream;
	unsigned char data[8];
	char correct[] = "10001010111111111111111100010010001101000101011001111000";

	testStart("Append Bytes");
	bstream = BitStream_new();

	data[0] = 0x8a;
	BitStream_appendBytes(bstream,  1, data);

	data[0] = 0xff;
	data[1] = 0xff;
	BitStream_appendBytes(bstream, 2, data);

	data[0] = 0x12;
	data[1] = 0x34;
	data[2] = 0x56;
	data[3] = 0x78;
	BitStream_appendBytes(bstream, 4, data);

	testEnd(strncmp(correct, bstream->data, 64));

	BitStream_free(bstream);
}

void test_toByte(void)
{
	BitStream *bstream;
	unsigned char correct[] = {
		0x8a, 0xff, 0xff, 0x12, 0x34, 0x56, 0x78
	};
	unsigned char *result;

	testStart("Convert to  byte array");
	bstream = BitStream_new();

	BitStream_appendBytes(bstream, 1, &correct[0]);
	BitStream_appendBytes(bstream, 2, &correct[1]);
	BitStream_appendBytes(bstream, 4, &correct[3]);

	result = BitStream_toByte(bstream);
	testEnd(memcmp(correct, result, 7));

	BitStream_free(bstream);
	free(result);
}

int main(int argc, char **argv)
{
	test_num();
	test_bytes();
	test_appendBytes();
	test_toByte();

	report();

	return 0;
}
