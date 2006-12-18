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

#ifndef __SPLIT_H__
#define __SPLIT_H__

#include "qrencode.h"
void Split_splitStringToQRinput(const char *string, QRinput *input,
		int version, QRencodeMode hint);
#endif /* __SPLIT_H__ */
