#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include "datachunk.h"

DataChunk *DataChunk_new(QRencodeMode mode)
{
	DataChunk *chunk;

	chunk = (DataChunk *)calloc(1, sizeof(DataChunk));
	if(chunk == NULL) return NULL;

	chunk->mode = mode;

	return chunk;
}

void DataChunk_free(DataChunk *chunk)
{
	if(chunk) {
		if(chunk->data) free(chunk->data);
		free(chunk);
	}
}

static void dumpNum(DataChunk *chunk)
{
	printf("%s\n", chunk->data);
}

static void dumpAn(DataChunk *chunk)
{
	printf("%s\n", chunk->data);
}

static void dump8(DataChunk *chunk)
{
	int i, j;
	unsigned char c;
	int count = 0;
	unsigned char buf[16];

	for(i=0; i<chunk->size; i++) {
		buf[count] = chunk->data[i];
		c = chunk->data[i];
		if(c >= ' ' && c <= '~') {
			putchar(c);
		} else {
			putchar('.');
		}
		count++;

		if(count >= 16) {
			putchar(' ');
			for(j=0; j<16; j++) {
				printf(" %02x", buf[j]);
			}
			count = 0;
			putchar('\n');
		}
	}
	if(count > 0) {
		for(i=0; i<16 - count; i++) {
			putchar(' ');
		}
		putchar(' ');
		for(j=0; j<count; j++) {
			printf(" %02x", buf[j]);
		}
		count = 0;
		putchar('\n');
	}
}

static void dumpKanji(DataChunk *chunk)
{
	iconv_t conv;
	char *inbuf, *outbuf, *outp;
	size_t inbytes, outbytes, ret;

	conv = iconv_open("UTF-8", "SHIFT_JIS");
	inbytes = chunk->size;
	inbuf = (char *)chunk->data;
	outbytes = inbytes * 4 + 1;
	outbuf = (char *)malloc(inbytes * 4 + 1);
	outp = outbuf;
	ret = iconv(conv, &inbuf, &inbytes, &outp, &outbytes);
	if(ret == (size_t) -1) { perror(NULL); }
	*outp = '\0';

	printf("%s\n", outbuf);

	iconv_close(conv);
	free(outbuf);
}

static void dumpChunk(DataChunk *chunk)
{
	switch(chunk->mode) {
		case QR_MODE_NUM:
			printf("Numeric: %d bytes\n", chunk->size);
			dumpNum(chunk);
			break;
		case QR_MODE_AN:
			printf("AlphaNumeric: %d bytes\n", chunk->size);
			dumpAn(chunk);
			break;
		case QR_MODE_8:
			printf("8-bit data: %d bytes\n", chunk->size);
			dump8(chunk);
			break;
		case QR_MODE_KANJI:
			printf("Kanji: %d bytes\n", chunk->size);
			dumpKanji(chunk);
			break;
		default:
			printf("Invalid or reserved: %d bytes\n", chunk->size);
			dump8(chunk);
			break;
	}
}

void DataChunk_dumpChunkList(DataChunk *list)
{
	while(list != NULL) {
		dumpChunk(list);
		list = list->next;
	}
}
