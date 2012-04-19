/**
 * qrencode - QR Code encoder
 *
 * QR Code encoding tool
 * Copyright (C) 2006-2012 Kentaro Fukuchi <kentaro@fukuchi.org>
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

#if HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <getopt.h>

#include "qrencode.h"

#define INCHES_PER_METER (100.0/2.54)

static int casesensitive = 1;
static int eightbit = 0;
static int version = 0;
static int size = 3;
static int margin = -1;
static int dpi = 72;
static int structured = 0;
static int micro = 0;
static QRecLevel level = QR_ECLEVEL_L;
static QRencodeMode hint = QR_MODE_8;

enum imageType {
	PNG_TYPE,
	EPS_TYPE,
	SVG_TYPE,
	ANSI_TYPE,
	ANSI256_TYPE,
	ASCII_TYPE,
	ASCIIi_TYPE
};

static enum imageType image_type = PNG_TYPE;

static const struct option options[] = {
	{"help"         , no_argument      , NULL, 'h'},
	{"output"       , required_argument, NULL, 'o'},
	{"level"        , required_argument, NULL, 'l'},
	{"size"         , required_argument, NULL, 's'},
	{"symversion"   , required_argument, NULL, 'v'},
	{"margin"       , required_argument, NULL, 'm'},
	{"dpi"          , required_argument, NULL, 'd'},
	{"type"         , required_argument, NULL, 't'},
	{"structured"   , no_argument      , NULL, 'S'},
	{"kanji"        , no_argument      , NULL, 'k'},
	{"casesensitive", no_argument      , NULL, 'c'},
	{"ignorecase"   , no_argument      , NULL, 'i'},
	{"8bit"         , no_argument      , NULL, '8'},
	{"micro"        , no_argument      , NULL, 'M'},
	{"version"      , no_argument      , NULL, 'V'},
	{NULL, 0, NULL, 0}
};

static char *optstring = "ho:l:s:v:m:d:t:Skci8MV";

static void usage(int help, int longopt)
{
	fprintf(stderr,
"qrencode version %s\n"
"Copyright (C) 2006-2012 Kentaro Fukuchi\n", QRcode_APIVersionString());
	if(help) {
		if(longopt) {
			fprintf(stderr,
"Usage: qrencode [OPTION]... [STRING]\n"
"Encode input data in a QR Code and save as a PNG or EPS image.\n\n"
"  -h, --help   display the help message. -h displays only the help of short\n"
"               options.\n\n"
"  -o FILENAME, --output=FILENAME\n"
"               write image to FILENAME. If '-' is specified, the result\n"
"               will be output to standard output. If -S is given, structured\n"
"               symbols are written to FILENAME-01.png, FILENAME-02.png, ...\n"
"               (suffix is removed from FILENAME, if specified)\n"
"  -s NUMBER, --size=NUMBER\n"
"               specify module size in dots (pixels). (default=3)\n\n"
"  -l {LMQH}, --level={LMQH}\n"
"               specify error correction level from L (lowest) to H (highest).\n"
"               (default=L)\n\n"
"  -v NUMBER, --symversion=NUMBER\n"
"               specify the version of the symbol. (default=auto)\n\n"
"  -m NUMBER, --margin=NUMBER\n"
"               specify the width of the margins. (default=4 (2 for Micro)))\n\n"
"  -d NUMBER, --dpi=NUMBER\n"
"               specify the DPI of the generated PNG. (default=72)\n\n"
"  -t {PNG,EPS,SVG,ANSI,ANSI256,ASCII}, --type={PNG,EPS,SVG,ANSI,ANSI256,ASCII}\n"
"               specify the type of the generated image. (default=PNG)\n\n"
"  -S, --structured\n"
"               make structured symbols. Version must be specified.\n\n"
"  -k, --kanji  assume that the input text contains kanji (shift-jis).\n\n"
"  -c, --casesensitive\n"
"               encode lower-case alphabet characters in 8-bit mode. (default)\n\n"
"  -i, --ignorecase\n"
"               ignore case distinctions and use only upper-case characters.\n\n"
"  -8, --8bit   encode entire data in 8-bit mode. -k, -c and -i will be ignored.\n\n"
"  -M, --micro  encode in a Micro QR Code. (experimental)\n\n"
"  -V, --version\n"
"               display the version number and copyrights of the qrencode.\n\n"
"  [STRING]     input data. If it is not specified, data will be taken from\n"
"               standard input.\n"
			);
		} else {
			fprintf(stderr,
"Usage: qrencode [OPTION]... [STRING]\n"
"Encode input data in a QR Code and save as a PNG or EPS image.\n\n"
"  -h           display this message.\n"
"  --help       display the usage of long options.\n"
"  -o FILENAME  write image to FILENAME. If '-' is specified, the result\n"
"               will be output to standard output. If -S is given, structured\n"
"               symbols are written to FILENAME-01.png, FILENAME-02.png, ...\n"
"               (suffix is removed from FILENAME, if specified)\n"
"  -s NUMBER    specify module size in dots (pixels). (default=3)\n"
"  -l {LMQH}    specify error correction level from L (lowest) to H (highest).\n"
"               (default=L)\n"
"  -v NUMBER    specify the version of the symbol. (default=auto)\n"
"  -m NUMBER    specify the width of the margins. (default=4 (2 for Micro))\n"
"  -d NUMBER    specify the DPI of the generated PNG. (default=72)\n"
"  -t {PNG,EPS,SVG,ANSI,ANSI256,ASCII}\n"
"               specify the type of the generated image. (default=PNG)\n"
"  -S           make structured symbols. Version must be specified.\n"
"  -k           assume that the input text contains kanji (shift-jis).\n"
"  -c           encode lower-case alphabet characters in 8-bit mode. (default)\n"
"  -i           ignore case distinctions and use only upper-case characters.\n"
"  -8           encode entire data in 8-bit mode. -k, -c and -i will be ignored.\n"
"  -M           encode in a Micro QR Code.\n"
"  -V           display the version number and copyrights of the qrencode.\n"
"  [STRING]     input data. If it is not specified, data will be taken from\n"
"               standard input.\n"
			);
		}
	}
}

#define MAX_DATA_SIZE (7090 * 16) /* from the specification */
static unsigned char *readStdin(int *length)
{
	unsigned char *buffer;
	int ret;

	buffer = (unsigned char *)malloc(MAX_DATA_SIZE + 1);
	if(buffer == NULL) {
		fprintf(stderr, "Memory allocation failed.\n");
		exit(EXIT_FAILURE);
	}
	ret = fread(buffer, 1, MAX_DATA_SIZE, stdin);
	if(ret == 0) {
		fprintf(stderr, "No input data.\n");
		exit(EXIT_FAILURE);
	}
	if(feof(stdin) == 0) {
		fprintf(stderr, "Input data is too large.\n");
		exit(EXIT_FAILURE);
	}

	buffer[ret] = '\0';
	*length = ret;

	return buffer;
}

static FILE *openFile(const char *outfile)
{
	FILE *fp;

	if(outfile == NULL || (outfile[0] == '-' && outfile[1] == '\0')) {
		fp = stdout;
	} else {
		fp = fopen(outfile, "wb");
		if(fp == NULL) {
			fprintf(stderr, "Failed to create file: %s\n", outfile);
			perror(NULL);
			exit(EXIT_FAILURE);
		}
	}

	return fp;
}

static int writePNG(QRcode *qrcode, const char *outfile)
{
	static FILE *fp; // avoid clobbering by setjmp.
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned char *row, *p, *q;
	int x, y, xx, yy, bit;
	int realwidth;

	realwidth = (qrcode->width + margin * 2) * size;
	row = (unsigned char *)malloc((realwidth + 7) / 8);
	if(row == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(EXIT_FAILURE);
	}

	if(outfile[0] == '-' && outfile[1] == '\0') {
		fp = stdout;
	} else {
		fp = fopen(outfile, "wb");
		if(fp == NULL) {
			fprintf(stderr, "Failed to create file: %s\n", outfile);
			perror(NULL);
			exit(EXIT_FAILURE);
		}
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL) {
		fprintf(stderr, "Failed to initialize PNG writer.\n");
		exit(EXIT_FAILURE);
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == NULL) {
		fprintf(stderr, "Failed to initialize PNG write.\n");
		exit(EXIT_FAILURE);
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fprintf(stderr, "Failed to write PNG image.\n");
		exit(EXIT_FAILURE);
	}

	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr,
			realwidth, realwidth,
			1,
			PNG_COLOR_TYPE_GRAY,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);
	png_set_pHYs(png_ptr, info_ptr,
			dpi * INCHES_PER_METER,
			dpi * INCHES_PER_METER,
			PNG_RESOLUTION_METER);
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

static int writeEPS(QRcode *qrcode, const char *outfile)
{
	FILE *fp;
	unsigned char *row, *p;
	int x, y, yy;
	int realwidth;

	fp = openFile(outfile);
   
	realwidth = (qrcode->width + margin * 2) * size;
	/* EPS file header */
	fprintf(fp, "%%!PS-Adobe-2.0 EPSF-1.2\n"
				"%%%%BoundingBox: 0 0 %d %d\n"
				"%%%%Pages: 1 1\n"
				"%%%%EndComments\n", realwidth, realwidth);
	/* draw point */
	fprintf(fp, "/p { "
				"moveto "
				"0 1 rlineto "
				"1 0 rlineto "
				"0 -1 rlineto "
				"fill "
				"} bind def "
				"%d %d scale ", size, size);
	
	/* data */
	p = qrcode->data;
	for(y=0; y<qrcode->width; y++) {
		row = (p+(y*qrcode->width));
		yy = (margin + qrcode->width - y - 1);
		
		for(x=0; x<qrcode->width; x++) {
			if(*(row+x)&0x1) {
				fprintf(fp, "%d %d p ", margin + x,  yy);
			}
		}
	}

	fprintf(fp, "\n%%%%EOF\n");
	fclose(fp);

	return 0;
}

static int writeSVG(QRcode *qrcode, const char *outfile)
{
	FILE *fp;
	unsigned char *row, *p;
	int x, y, yy, xx;
	int realwidth;

	fp = openFile(outfile);

	realwidth = (qrcode->width + margin * 2) * size;
   
	/* SVG file header */
	fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%d\" height=\"%d\">\n\t<g>\n",
			realwidth, realwidth);

	/* data */
	p = qrcode->data;
	for(y=0; y<qrcode->width; y++) {
		row = (p+(y*qrcode->width));
		yy = (y + margin) * size;	
		for(x=0; x<qrcode->width; x++) {
			if(*(row+x)&0x1) {
				xx = (x + margin) * size;
				fprintf(fp, "\t\t<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" style=\"fill:black\"/>\n", xx,  yy, size, size);
			}
		}
	}

	fprintf(fp, "\t</g>\n</svg>");
	fclose(fp);

	return 0;
}

static void writeANSI_margin(FILE* fp, int realwidth,
                             char* buffer, int buffer_s,
                             char* white, int white_s )
{
	int y;

	strncpy(buffer, white, white_s);
	memset(buffer + white_s, ' ', realwidth * 2);
	strcpy(buffer + white_s + realwidth * 2, "\033[0m\n"); // reset to default colors
	for(y=0; y<margin; y++ ){
		fputs(buffer, fp);
	}
}

static int writeANSI(QRcode *qrcode, const char *outfile)
{
	FILE *fp;
	unsigned char *row, *p;
	int x, y;
	int realwidth;
	int last;

	char *white, *black, *buffer;
	int white_s, black_s, buffer_s;

	if( image_type == ANSI256_TYPE ){
		/* codes for 256 color compatible terminals */
		white = "\033[48;5;231m";
		white_s = 11;
		black = "\033[48;5;16m";
		black_s = 10;
	} else {
		white = "\033[47m";
		white_s = 5;
		black = "\033[40m";
		black_s = 5;
	}

	size = 1;

	fp = openFile(outfile);

	realwidth = (qrcode->width + margin * 2) * size;
	buffer_s = ( realwidth * white_s ) * 2;
	buffer = (char *)malloc( buffer_s );
	if(buffer == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(EXIT_FAILURE);
	}

	/* top margin */
	writeANSI_margin(fp, realwidth, buffer, buffer_s, white, white_s);

	/* data */
	p = qrcode->data;
	for(y=0; y<qrcode->width; y++) {
		row = (p+(y*qrcode->width));

		bzero( buffer, buffer_s );
		strncpy( buffer, white, white_s );
		for(x=0; x<margin; x++ ){
			strncat( buffer, "  ", 2 );
		}
		last = 0;

		for(x=0; x<qrcode->width; x++) {
			if(*(row+x)&0x1) {
				if( last != 1 ){
					strncat( buffer, black, black_s );
					last = 1;
				}
			} else {
				if( last != 0 ){
					strncat( buffer, white, white_s );
					last = 0;
				}
			}
			strncat( buffer, "  ", 2 );
		}

		if( last != 0 ){
			strncat( buffer, white, white_s );
		}
		for(x=0; x<margin; x++ ){
			strncat( buffer, "  ", 2 );
		}
		strncat( buffer, "\033[0m\n", 5 );
		fputs( buffer, fp );
	}

	/* bottom margin */
	writeANSI_margin(fp, realwidth, buffer, buffer_s, white, white_s);

	fclose(fp);
	free(buffer);

	return 0;
}

static void writeASCII_margin(FILE* fp, int realwidth, char* buffer, int buffer_s, int invert)
{
	int y, h;

	h = margin;

	memset(buffer, (invert?'#':' '), realwidth);
	buffer[realwidth] = '\n';
	buffer[realwidth + 1] = '\0';
	for(y=0; y<h; y++ ){
		fputs(buffer, fp);
	}
}

static int writeASCII(QRcode *qrcode, const char *outfile, int invert)
{
	FILE *fp;
	unsigned char *row;
	int x, y;
	int realwidth;
	char *buffer, *p;
	int buffer_s;
	char black = '#';
	char white = ' ';

	if(invert) {
		black = ' ';
		white = '#';
	}

	size = 1;

	fp = openFile(outfile);

	realwidth = (qrcode->width + margin * 2) * 2;
	buffer_s = realwidth + 1;
	buffer = (char *)malloc( buffer_s );
	if(buffer == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(EXIT_FAILURE);
	}

	/* top margin */
	writeASCII_margin(fp, realwidth, buffer, buffer_s, invert);

	/* data */
	for(y=0; y<qrcode->width; y++) {
		row = qrcode->data+(y*qrcode->width);
		p = buffer;

		memset(p, white, margin * 2);
		p += margin * 2;

		for(x=0; x<qrcode->width; x++) {
			if(row[x]&0x1) {
				*p++ = black;
				*p++ = black;
			} else {
				*p++ = white;
				*p++ = white;
			}
		}

		memset(p, white, margin * 2);
		p += margin * 2;
		*p++ = '\n';
		*p++ = '\0';
		fputs( buffer, fp );
	}

	/* bottom margin */
	writeASCII_margin(fp, realwidth, buffer, buffer_s, invert);

	fclose(fp);
	free(buffer);

	return 0;
}

static QRcode *encode(const unsigned char *intext, int length)
{
	QRcode *code;

	if(micro) {
		if(eightbit) {
			code = QRcode_encodeDataMQR(length, intext, version, level);
		} else {
			code = QRcode_encodeStringMQR((char *)intext, version, level, hint, casesensitive);
		}
	} else {
		if(eightbit) {
			code = QRcode_encodeData(length, intext, version, level);
		} else {
			code = QRcode_encodeString((char *)intext, version, level, hint, casesensitive);
		}
	}

	return code;
}

static void qrencode(const unsigned char *intext, int length, const char *outfile)
{
	QRcode *qrcode;
	
	qrcode = encode(intext, length);
	if(qrcode == NULL) {
		perror("Failed to encode the input data");
		exit(EXIT_FAILURE);
	}
	switch(image_type) {
		case PNG_TYPE:
			writePNG(qrcode, outfile);
			break;
		case EPS_TYPE:
			writeEPS(qrcode, outfile);
			break;
		case SVG_TYPE:
			writeSVG(qrcode, outfile);
			break;
		case ANSI_TYPE:
		case ANSI256_TYPE:
			writeANSI(qrcode, outfile);
			break;
		case ASCIIi_TYPE:
			writeASCII(qrcode, outfile,  1);
			break;
		case ASCII_TYPE:
			writeASCII(qrcode, outfile,  0);
			break;
		default:
			fprintf(stderr, "Unknown image type.\n");
			exit(EXIT_FAILURE);
	}
	QRcode_free(qrcode);
}

static QRcode_List *encodeStructured(const unsigned char *intext, int length)
{
	QRcode_List *list;

	if(eightbit) {
		list = QRcode_encodeDataStructured(length, intext, version, level);
	} else {
		list = QRcode_encodeStringStructured((char *)intext, version, level, hint, casesensitive);
	}

	return list;
}

static void qrencodeStructured(const unsigned char *intext, int length, const char *outfile)
{
	QRcode_List *qrlist, *p;
	char filename[FILENAME_MAX];
	char *base, *q, *suffix = NULL;
	const char *type_suffix;
	int i = 1;
	size_t suffix_size;

	switch(image_type) {
		case PNG_TYPE:
			type_suffix = ".png";
			break;
		case EPS_TYPE:
			type_suffix = ".eps";
			break;
		case SVG_TYPE:
			type_suffix = ".svg";
			break;
		case ANSI_TYPE:
		case ANSI256_TYPE:
		case ASCII_TYPE:
			type_suffix = ".txt";
			break;
		default:
			fprintf(stderr, "Unknown image type.\n");
			exit(EXIT_FAILURE);
	}

	if(outfile == NULL) {
		fprintf(stderr, "An output filename must be specified to store the structured images.\n");
		exit(EXIT_FAILURE);
	}
	base = strdup(outfile);
	if(base == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(EXIT_FAILURE);
	}
	suffix_size = strlen(type_suffix);
	if(strlen(base) > suffix_size) {
		q = base + strlen(base) - suffix_size;
		if(strcasecmp(type_suffix, q) == 0) {
			suffix = strdup(q);
			*q = '\0';
		}
	}
	
	qrlist = encodeStructured(intext, length);
	if(qrlist == NULL) {
		perror("Failed to encode the input data");
		exit(EXIT_FAILURE);
	}

	for(p = qrlist; p != NULL; p = p->next) {
		if(p->code == NULL) {
			fprintf(stderr, "Failed to encode the input data.\n");
			exit(EXIT_FAILURE);
		}
		if(suffix) {
			snprintf(filename, FILENAME_MAX, "%s-%02d%s", base, i, suffix);
		} else {
			snprintf(filename, FILENAME_MAX, "%s-%02d", base, i);
		}
		switch(image_type) {
			case PNG_TYPE: 
				writePNG(p->code, filename);
				break;
			case EPS_TYPE: 
				writeEPS(p->code, filename);
				break;
			case SVG_TYPE: 
				writeSVG(p->code, filename);
				break;
			case ANSI_TYPE:
			case ANSI256_TYPE:
				writeANSI(p->code, filename);
				break;
			case ASCIIi_TYPE:
				writeASCII(p->code, filename, 1);
				break;
			case ASCII_TYPE:
				writeASCII(p->code, filename, 0);
				break;
			default:
				fprintf(stderr, "Unknown image type.\n");
				exit(EXIT_FAILURE);
		}
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
	int opt, lindex = -1;
	char *outfile = NULL;
	unsigned char *intext = NULL;
	int length = 0;

	while((opt = getopt_long(argc, argv, optstring, options, &lindex)) != -1) {
		switch(opt) {
			case 'h':
				if(lindex == 0) {
					usage(1, 1);
				} else {
					usage(1, 0);
				}
				exit(EXIT_SUCCESS);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 's':
				size = atoi(optarg);
				if(size <= 0) {
					fprintf(stderr, "Invalid size: %d\n", size);
					exit(EXIT_FAILURE);
				}
				break;
			case 'v':
				version = atoi(optarg);
				if(version < 0) {
					fprintf(stderr, "Invalid version: %d\n", version);
					exit(EXIT_FAILURE);
				}
				break;
			case 'l':
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
						exit(EXIT_FAILURE);
						break;
				}
				break;
			case 'm':
				margin = atoi(optarg);
				if(margin < 0) {
					fprintf(stderr, "Invalid margin: %d\n", margin);
					exit(EXIT_FAILURE);
				}
				break;
			case 'd':
				dpi = atoi(optarg);
				if( dpi < 0 ) {
					fprintf(stderr, "Invalid DPI: %d\n", dpi);
					exit(EXIT_FAILURE);
				}
				break;
			case 't':
				if(strcasecmp(optarg, "png") == 0) {
					image_type = PNG_TYPE;
				} else if(strcasecmp(optarg, "eps") == 0) {
					image_type = EPS_TYPE;
				} else if(strcasecmp(optarg, "svg") == 0) {
					image_type = SVG_TYPE;
				} else if(strcasecmp(optarg, "ansi") == 0) {
					image_type = ANSI_TYPE;
				} else if(strcasecmp(optarg, "ansi256") == 0) {
					image_type = ANSI256_TYPE;
				} else if(strcasecmp(optarg, "asciii") == 0) {
					image_type = ASCIIi_TYPE;
				} else if(strcasecmp(optarg, "ascii") == 0) {
					image_type = ASCII_TYPE;
				} else {
					fprintf(stderr, "Invalid image type: %s\n", optarg);
					exit(EXIT_FAILURE);
				}
				break;
			case 'S':
				structured = 1;
			case 'k':
				hint = QR_MODE_KANJI;
				break;
			case 'c':
				casesensitive = 1;
				break;
			case 'i':
				casesensitive = 0;
				break;
			case '8':
				eightbit = 1;
				break;
			case 'M':
				micro = 1;
				break;
			case 'V':
				usage(0, 0);
				exit(EXIT_SUCCESS);
				break;
			default:
				fprintf(stderr, "Try `qrencode --help' for more information.\n");
				exit(EXIT_FAILURE);
				break;
		}
	}

	if(argc == 1) {
		usage(1, 0);
		exit(EXIT_SUCCESS);
	}

	if(outfile == NULL && image_type == PNG_TYPE) {
		fprintf(stderr, "No output filename is given.\n");
		exit(EXIT_FAILURE);
	}

	if(optind < argc) {
		intext = (unsigned char *)argv[optind];
		length = strlen((char *)intext);
	}
	if(intext == NULL) {
		intext = readStdin(&length);
	}

	if(micro && version > MQRSPEC_VERSION_MAX) {
		fprintf(stderr, "Version should be less or equal to %d.\n", MQRSPEC_VERSION_MAX);
		exit(EXIT_FAILURE);
	} else if(!micro && version > QRSPEC_VERSION_MAX) {
		fprintf(stderr, "Version should be less or equal to %d.\n", QRSPEC_VERSION_MAX);
		exit(EXIT_FAILURE);
	}

	if(margin < 0) {
		if(micro) {
			margin = 2;
		} else {
			margin = 4;
		}
	}

	if(micro) {
		if(version == 0) {
			fprintf(stderr, "Version must be specified to encode a Micro QR Code symbol.\n");
			exit(EXIT_FAILURE);
		}
		if(structured) {
			fprintf(stderr, "Micro QR Code does not support structured symbols.\n");
			exit(EXIT_FAILURE);
		}
	}

	if(structured) {
		if(version == 0) {
			fprintf(stderr, "Version must be specified to encode structured symbols.\n");
			exit(EXIT_FAILURE);
		}
		qrencodeStructured(intext, length, outfile);
	} else {
		qrencode(intext, length, outfile);
	}

	return 0;
}
