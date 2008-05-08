/**
 * qrencode - QR Code encoder
 *
 * QR Code encoding tool
 * Copyright (C) 2006, 2007, 2008 Kentaro Fukuchi <fukuchi@megaui.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <getopt.h>

#include "qrencode.h"

static int casesensitive = 1;
static int eightbit = 0;
static int version = 0;
static int size = 3;
static int margin = 4;
static int structured = 0;
static QRecLevel level = QR_ECLEVEL_L;
static QRencodeMode hint = QR_MODE_8;

enum {
	O_HELP,
	O_OUTPUT,
	O_SIZE,
	O_VERSION,
	O_LEVEL,
	O_MARGIN,
	O_KANJI,
	O_CASE,
	O_IGNORECASE,
	O_8BIT,
	O_STRUCTURED,
};

static const struct option options[] = {
	{"h", no_argument      , NULL, O_HELP},
	{"o", required_argument, NULL, O_OUTPUT},
	{"l", required_argument, NULL, O_LEVEL},
	{"s", required_argument, NULL, O_SIZE},
	{"v", required_argument, NULL, O_VERSION},
	{"m", required_argument, NULL, O_MARGIN},
	{"k", no_argument      , NULL, O_KANJI},
	{"c", no_argument      , NULL, O_CASE},
	{"i", no_argument      , NULL, O_IGNORECASE},
	{"8", no_argument      , NULL, O_8BIT},
	{"S", no_argument      , NULL, O_STRUCTURED},
	{NULL, 0, NULL, 0}
};

static void usage(void)
{
	fprintf(stderr,
"qrencode version %s\n"
"Copyright (C) 2006, 2007, 2008 Kentaro Fukuchi\n"
"Usage: qrencode [OPTION]... [STRING]\n"
"Encode input data in a QR Code and save as a PNG image.\n\n"
"  -h           display this message.\n"
"  -o FILENAME  write PNG image to FILENAME. If '-' is specified, the result\n"
"               will be output to standard output. If -S is given, structured\n"
"               symbols are written to FILENAME-01.png, FILENAME-02.png, ...;\n"
"               if specified, remove a trailing '.png' from FILENAME.\n"
"  -s NUMBER    specify the size of dot (pixel). (default=3)\n"
"  -l {LMQH}    specify error collectin level from L (lowest) to H (highest).\n"
"               (default=L)\n"
"  -v NUMBER    specify the version of the symbol. (default=auto)\n"
"  -m NUMBER    specify the width of margin. (default=4)\n"
"  -S           make structured symbols. Version must be specified.\n"
"  -k           assume that the input text contains kanji (shift-jis).\n"
"  -c           encode lower-case alphabet characters in 8-bit mode. (default)\n"
"  -i           ignore case distinctions and use only upper-case characters.\n"
"  -8           encode entire data in 8-bit mode. -k, -c and -i will be ignored.\n"
"  [STRING]     input data. If it is not specified, data will be taken from\n"
"               standard input.\n",
	VERSION);
}

#define MAX_DATA_SIZE (7090 * 16) /* from the specification */
static char *readStdin(void)
{
	char *buffer;
	int ret;

	buffer = (char *)malloc(MAX_DATA_SIZE);
	ret = fread(buffer, 1, MAX_DATA_SIZE, stdin);
	if(ret == 0) {
		fprintf(stderr, "No input data.\n");
		exit(1);
	}
	if(!feof(stdin)) {
		fprintf(stderr, "Input data is too large.\n");
		exit(1);
	}

	buffer[ret] = '\0';

	return buffer;
}

static int writePNG(QRcode *qrcode, const char *outfile)
{
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned char *row, *p, *q;
	int x, y, xx, yy, bit;
	int realwidth;

	realwidth = (qrcode->width + margin * 2) * size;
	row = (unsigned char *)malloc((realwidth + 7) / 8);
	if(row == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(1);
	}

	if(outfile[0] == '-' && outfile[1] == '\0') {
		fp = stdout;
	} else {
		fp = fopen(outfile, "wb");
		if(fp == NULL) {
			fprintf(stderr, "Failed to create file: %s\n", outfile);
			perror(NULL);
			exit(1);
		}
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL) {
		fclose(fp);
		fprintf(stderr, "Failed to initialize PNG writer.\n");
		exit(1);
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == NULL) {
		fclose(fp);
		fprintf(stderr, "Failed to initialize PNG write.\n");
		exit(1);
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		fprintf(stderr, "Failed to write PNG image.\n");
		exit(1);
	}

	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr,
			realwidth, realwidth,
			1,
			PNG_COLOR_TYPE_GRAY,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png_ptr, info_ptr);

	/* top margin */
	memset(row, 0xff, (realwidth + 7) / 8);
	for(y=0; y<margin * size; y++) {
		png_write_row(png_ptr, row);
	}

	/* data */
	p = qrcode->data;
	for(y=0; y<qrcode->width; y++) {
		bit = 7;
		memset(row, 0xff, (realwidth + 7) / 8);
		q = row;
		q += margin * size / 8;
		bit = 7 - (margin * size % 8);
		for(x=0; x<qrcode->width; x++) {
			for(xx=0; xx<size; xx++) {
				*q ^= (*p & 1) << bit;
				bit--;
				if(bit < 0) {
					q++;
					bit = 7;
				}
			}
			p++;
		}
		for(yy=0; yy<size; yy++) {
			png_write_row(png_ptr, row);
		}
	}
	/* bottom margin */
	memset(row, 0xff, (realwidth + 7) / 8);
	for(y=0; y<margin * size; y++) {
		png_write_row(png_ptr, row);
	}

	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);
	free(row);

	return 0;
}

static QRcode *encode(const char *intext)
{
	QRcode *code;

	if(eightbit) {
		code = QRcode_encodeString8bit(intext, version, level);
	} else {
		code = QRcode_encodeString(intext, version, level, hint, casesensitive);
	}

	return code;
}

static void qrencode(const char *intext, const char *outfile)
{
	QRcode *qrcode;
	
	qrcode = encode(intext);
	if(qrcode == NULL) {
		fprintf(stderr, "Failed to encode the input data.\n");
		exit(1);
	}
	writePNG(qrcode, outfile);
	QRcode_free(qrcode);
}

static QRcode_List *encodeStructured(const char *intext)
{
	QRcode_List *list;

	if(eightbit) {
		list = QRcode_encodeString8bitStructured(intext, version, level);
	} else {
		list = QRcode_encodeStringStructured(intext, version, level, hint, casesensitive);
	}

	return list;
}

static void qrencodeStructured(const char *intext, const char *outfile)
{
	QRcode_List *qrlist, *p;
	char filename[FILENAME_MAX];
	char *base, *q, *suffix = NULL;
	int i = 1;

	base = strdup(outfile);
	if(base == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(1);
	}
	if(strlen(base) > 4) {
		q = base + strlen(base) - 4;
		if(strcasecmp(".png", q) == 0) {
			suffix = strdup(q);
			*q = '\0';
		}
	}
	
	qrlist = encodeStructured(intext);
	if(qrlist == NULL) {
		fprintf(stderr, "Failed to encode the input data.\n");
		exit(1);
	}

	for(p = qrlist; p != NULL; p = p->next) {
		if(p->code == NULL) {
			fprintf(stderr, "Failed to encode the input data.\n");
			exit(1);
		}
		if(suffix) {
			snprintf(filename, FILENAME_MAX, "%s-%02d%s", base, i, suffix);
		} else {
			snprintf(filename, FILENAME_MAX, "%s-%02d", base, i);
		}
		writePNG(p->code, filename);
		i++;
	}

	free(base);
	if(suffix) {
		free(suffix);
	}

	QRcode_List_free(qrlist);
}

int main(int argc, char **argv)
{
	int opt;
	char *outfile = NULL;
	char *intext = NULL;

	while((opt = getopt_long_only(argc, argv, "", options, NULL)) != -1) {
		switch(opt) {
			case O_HELP:
				usage();
				exit(0);
				break;
			case O_OUTPUT:
				outfile = optarg;
				break;
			case O_SIZE:
				size = atoi(optarg);
				if(size <= 0) {
					fprintf(stderr, "Invalid size: %d\n", size);
					exit(1);
				}
				break;
			case O_VERSION:
				version = atoi(optarg);
				if(version < 0) {
					fprintf(stderr, "Invalid version: %d\n", version);
					exit(1);
				}
				break;
			case O_LEVEL:
				switch(*optarg) {
					case 'l':
					case 'L':
						level = QR_ECLEVEL_L;
						break;
					case 'm':
					case 'M':
						level = QR_ECLEVEL_M;
						break;
					case 'q':
					case 'Q':
						level = QR_ECLEVEL_Q;
						break;
					case 'h':
					case 'H':
						level = QR_ECLEVEL_H;
						break;
					default:
						fprintf(stderr, "Invalid level: %s\n", optarg);
						exit(1);
						break;
				}
				break;
			case O_MARGIN:
				margin = atoi(optarg);
				if(margin < 0) {
					fprintf(stderr, "Invalid margin: %d\n", margin);
					exit(1);
				}
				break;
			case O_STRUCTURED:
				structured = 1;
			case O_KANJI:
				hint = QR_MODE_KANJI;
				break;
			case O_CASE:
				casesensitive = 1;
				break;
			case O_IGNORECASE:
				casesensitive = 0;
				break;
			case O_8BIT:
				eightbit = 1;
				break;
			default:
				usage();
				exit(1);
				break;
		}
	}

	if(argc == 1) {
		usage();
		exit(0);
	}

	if(outfile == NULL) {
		fprintf(stderr, "No output filename is given.\n");
		exit(1);
	}

	if(optind < argc) {
		intext = argv[optind];
	}
	if(intext == NULL) {
		intext = readStdin();
	}

	if(structured) {
		if(version == 0) {
			fprintf(stderr, "Version must be specified to encode structured symbols.\n");
			exit(1);
		}
		qrencodeStructured(intext, outfile);
	} else {
		qrencode(intext, outfile);
	}

	return 0;
}
