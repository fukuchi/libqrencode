/*
 * qrencode - QR Code encoder
 *
 * Input data splitter.
 * Copyright (C) 2006 Kentaro Fukuchi <fukuchi@megaui.net>
 *
 * The following data / specifications are taken from
 * "Two dimensional symbol -- QR-code -- Basic Specification" (JIS X0510:2004)
 *  or
 * "Automatic identification and data capture techniques -- 
 *  QR Code 2005 bar code symbology specification" (ISO/IEC 18004:2006)
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

#include "qrencode.h"
#include "qrinput.h"
#include "qrspec.h"
#include "split.h"

#define isdigit(__c__) ((unsigned char)((signed char)(__c__) - '0') < 10)
#define isalnum(__c__) (QRinput_lookAnTable(__c__) >= 0)

static int Split_eatNum(const char *string, QRinput *input, int version, QRencodeMode hint);
static int Split_eatAn(const char *string, QRinput *input, int version, QRencodeMode hint);
static int Split_eat8(const char *string, QRinput *input, int version, QRencodeMode hint);
static int Split_eatKanji(const char *string, QRinput *input, int version, QRencodeMode hint);

static int Split_eatNum(const char *string, QRinput *input, int version, QRencodeMode hint)
{
	const char *p;
	int run;
	int dif;
	int ln;

	ln = QRspec_lengthIndicator(QR_MODE_NUM, version);

	p = string;
	while(isdigit(*p)) {
		p++;
	}
	run = p - string;
	if(*p & 0x80) {
		dif = QRinput_estimateBitsModeNum(run) + 4 + ln
			+ QRinput_estimateBitsMode8(1) /* + 4 + l8 */
			- QRinput_estimateBitsMode8(run + 1) /* - 4 - l8 */;
		if(dif > 0) {
			return Split_eat8(string, input, version, hint);
		}
	}
	if(isalnum(*p)) {
		dif = QRinput_estimateBitsModeNum(run) + 4 + ln
			+ QRinput_estimateBitsModeAn(1) /* + 4 + la */
			- QRinput_estimateBitsModeAn(run + 1) /* - 4 - la */;
		if(dif > 0) {
			return Split_eatAn(string, input, version, hint);
		}
	}

	QRinput_append(input, QR_MODE_NUM, run, (unsigned char *)string);
	return run;
}

static int Split_eatAn(const char *string, QRinput *input, int version, QRencodeMode hint)
{
	const char *p, *q;
	int run;
	int dif;
	int la, ln;

	la = QRspec_lengthIndicator(QR_MODE_AN, version);
	ln = QRspec_lengthIndicator(QR_MODE_NUM, version);

	p = string;
	while(isalnum(*p)) {
		if(isdigit(*p)) {
			q = p;
			while(isdigit(*q)) {
				q++;
			}
			dif = QRinput_estimateBitsModeAn(p - string) /* + 4 + la */
				+ QRinput_estimateBitsModeNum(q - p) + 4 + ln
				- QRinput_estimateBitsModeAn(q - string) /* - 4 - la */;
			if(dif < 0) {
				break;
			} else {
				p = q;
			}
		} else {
			p++;
		}
	}

	run = p - string;

	if(*p & 0x80) {
		dif = QRinput_estimateBitsModeAn(run) + 4 + la
			+ QRinput_estimateBitsMode8(1) /* + 4 + l8 */
			- QRinput_estimateBitsMode8(run + 1) /* - 4 - l8 */;
		if(dif > 0) {
			return Split_eat8(string, input, version, hint);
		}
	}

	QRinput_append(input, QR_MODE_AN, run, (unsigned char *)string);
	return run;
}

static int Split_eatKanji(const char *string, QRinput *input, int version, QRencodeMode hint)
{
	const char *p;

	p = string;
	while(QRinput_identifyMode(p) == QR_MODE_KANJI) {
		p += 2;
	}
	QRinput_append(input, QR_MODE_KANJI, p - string, (unsigned char *)string);
	return p - string;
}

static int Split_eat8(const char *string, QRinput *input, int version, QRencodeMode hint)
{
	const char *p, *q;
	QRencodeMode mode;
	int dif;
	int la, ln;

	la = QRspec_lengthIndicator(QR_MODE_AN, version);
	ln = QRspec_lengthIndicator(QR_MODE_NUM, version);

	p = string;
	while(*p != '\0') {
		mode = QRinput_identifyMode(p);
		if(hint == QR_MODE_KANJI && mode == QR_MODE_KANJI) {
			break;
		}
		if(mode != QR_MODE_8 && mode != QR_MODE_KANJI) {
			if(mode == QR_MODE_NUM) {
				q = p;
				while(isdigit(*q)) {
					q++;
				}
				dif = QRinput_estimateBitsMode8(p - string) /* + 4 + l8 */
					+ QRinput_estimateBitsModeNum(q - p) + 4 + ln
					- QRinput_estimateBitsMode8(q - string) /* - 4 - l8 */;
				if(dif < 0) {
					break;
				} else {
					p = q;
				}
			} else {
				q = p;
				while(isalnum(*q)) {
					q++;
				}
				dif = QRinput_estimateBitsMode8(p - string) /* + 4 + l8 */
					+ QRinput_estimateBitsModeAn(q - p) + 4 + la
					- QRinput_estimateBitsMode8(q - string) /* - 4 - l8 */;
				if(dif < 0) {
					break;
				} else {
					p = q;
				}
			}
		} else {
			p++;
		}
	}

	QRinput_append(input, QR_MODE_8, p - string, (unsigned char *)string);
	return p - string;
}

void Split_splitStringToQRinput(const char *string, QRinput *input,
		int version, QRencodeMode hint)
{
	int length;
	QRencodeMode mode;

	if(*string == '\0') return;

	mode = QRinput_identifyMode(string);
	if(mode == QR_MODE_NUM) {
		length = Split_eatNum(string, input, version, hint);
	} else if(mode == QR_MODE_AN) {
		length = Split_eatAn(string, input, version, hint);
	} else if(mode == QR_MODE_KANJI && hint == QR_MODE_KANJI) {
		length = Split_eatKanji(string, input, version, hint);
	} else {
		length = Split_eat8(string, input, version, hint);
	}
	if(length == 0) return;
	/* Of course this tail recursion could be optimized! Believe gcc. */
	Split_splitStringToQRinput(&string[length], input, hint, version);
}
