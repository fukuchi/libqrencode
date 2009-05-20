#include <stdio.h>
#include <string.h>
#include "common.h"
#include "../bitstream.h"

void test_null(void)
{
	BitStream *bstream;

	testStart("Empty stream");
	bstream = BitStream_new();
	assert_zero(BitStream_size(bstream), "Size of empty BitStream is not 0.");
	assert_null(BitStream_toByte(bstream), "BitStream_toByte returned non-NULL.");
	assert_nothing(BitStream_free(NULL), "Check BitStream_free(NULL).\n");
	testFinish();

	BitStream_free(bstream);
}

void test_num(void)
{
	BitStream *bstream;
	unsigned int data = 0x13579bdf;
	char correct[] = "0010011010101111001101111011111";

	testStart("New from num");
	bstream = BitStream_new();
	BitStream_appendNum(bstream, 31, data);
	testEnd(cmpBin(correct, bstream));

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
	testEnd(cmpBin(correct, bstream));
	BitStream_free(bstream);
}

void test_appendNum(void)
{
	BitStream *bstream;
	char correct[] = "10001010111111111111111100010010001101000101011001111000";

	testStart("Append Num");
	bstream = BitStream_new();

	BitStream_appendNum(bstream,  8, 0x0000008a);
	assert_zero(ncmpBin(correct, bstream, 8), "Internal data is incorrect.");

	BitStream_appendNum(bstream, 16, 0x0000ffff);
	assert_zero(ncmpBin(correct, bstream, 24), "Internal data is incorrect.");

	BitStream_appendNum(bstream, 32, 0x12345678);

	assert_zero(cmpBin(correct, bstream), "Internal data is incorrect.");
	testFinish();

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
	assert_zero(ncmpBin(correct, bstream, 8), "Internal data is incorrect.");

	data[0] = 0xff;
	data[1] = 0xff;
	BitStream_appendBytes(bstream, 2, data);
	assert_zero(ncmpBin(correct, bstream, 24), "Internal data is incorrect.");

	data[0] = 0x12;
	data[1] = 0x34;
	data[2] = 0x56;
	data[3] = 0x78;
	BitStream_appendBytes(bstream, 4, data);

	assert_zero(cmpBin(correct, bstream), "Internal data is incorrect.");
	testFinish();

	BitStream_free(bstream);
}

void test_toByte(void)
{
	BitStream *bstream;
	unsigned char correct[] = {
		0x8a, 0xff, 0xff, 0x12, 0x34, 0x56, 0x78
	};
	unsigned char *result;

	testStart("Convert to a byte array");
	bstream = BitStream_new();

	BitStream_appendBytes(bstream, 1, &correct[0]);
	BitStream_appendBytes(bstream, 2, &correct[1]);
	BitStream_appendBytes(bstream, 4, &correct[3]);

	result = BitStream_toByte(bstream);
	testEnd(memcmp(correct, result, 7));

	BitStream_free(bstream);
	free(result);
}

void test_size(void)
{
	BitStream *bstream;

	testStart("size check");
	bstream = BitStream_new();
	assert_equal(BitStream_size(bstream), 0, "Initialized BitStream is not 0 length");
	BitStream_appendNum(bstream, 1, 0);
	assert_equal(BitStream_size(bstream), 1, "Size incorrect. (first append)");
	BitStream_appendNum(bstream, 2, 0);
	assert_equal(BitStream_size(bstream), 3, "Size incorrect. (second append)");
	testFinish();

	BitStream_free(bstream);
}

int main(int argc, char **argv)
{
	test_null();
	test_num();
	test_bytes();
	test_appendNum();
	test_appendBytes();
	test_toByte();
	test_size();

	report();

	return 0;
}
