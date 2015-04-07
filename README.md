# libqrencode 3.9.0 - QR Code encoding library [![Build Status](https://travis-ci.org/fukuchi/libqrencode.png?branch=master)](https://travis-ci.org/fukuchi/libqrencode)

GENERAL INFORMATION
===================
Libqrencode is a library for encoding data in a QR Code symbol, a 2D symbology
that can be scanned by handy terminals such as a mobile phone with CCD. The
capacity of QR Code is up to 7000 digits or 4000 characters and has high
robustness.

Libqrencode accepts a string or a list of data chunks then encodes in a QR Code
symbol as a bitmap array. While other QR Code applications generate an image
file, using libqrencode allows applications to render QR Code symbols from raw
bitmap data directly. This library also contains a command-line utility outputs
a QR Code symbol as a PNG image.


SPECIFICATION
=============
Libqrencode supports QR Code model 2, described in JIS (Japanese Industrial
Standards) X0510:2004 or ISO/IEC 18004. Most of features in the specification
are implemented such as:
- Numeric, alphabet, Japanese kanji (Shift-JIS) or any 8 bit code can be
  embedded
- Optimized encoding of a string
- Structured-append of symbols
- Micro QR Code (experimental)

Currently the following features are not supported:
- ECI and FNC1 mode
- QR Code model 1 (deprecated)


INSTALL
=======

Requirements
------------
Some test programs or utility tools uses SDL or PNG, but the library itself
has no dependencies. You can skip compiling those tools if you want not to
install programs using SDL or PNG.

Compile & install
-----------------
Just try

```
./autogen.sh
./configure
make
make install
```

This compiles and installs the library and header file to the appropriate
directories. By default, /usr/local/lib and /usr/local/include. You can change
the destination directory by passing some options to the configure script.
Run "./configure --help" to see the list of options.

It also installs a binary "qrencode" to /usr/local/bin. If you want not to
install it, give "--without-tools" option to the configure script.

When you downloaded a development tree from github, it is required to run
"autogen.sh" at first to generate configure script.


USAGE
=====
Basic usages of this library are written in the header file (qrencode.h).
You can generate a manual of the library by using Doxygen.


WARNINGS
========
The library is distributed WITHOUT ANY WARRANTY.

Micro QR Code support is EXPERIMENTAL.

Be careful to use the command line tool (qrencode) if it is used by a web
application (e.g. CGI script). For example, giving "-s" option with a large
number to qrencode may cause DoS. The parameters should be checked by the
application.


LICENSING INFORMATION
=====================
Copyright (C) 2006-2012 Kentaro Fukuchi

This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 2.1 of the License, or any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this library; if not, write to the Free Software Foundation, Inc., 51
Franklin St, Fifth Floor, Boston, MA 02110-1301 USA


CONTACT
=======
Visit the homepage at:

http://fukuchi.org/works/qrencode/

for new releases. The git repository is available at:

https://github.com/fukuchi/libqrencode

Please mail any bug reports, suggestions, comments, and questions to:

Kentaro Fukuchi <kentaro@fukuchi.org>

or submit issues to:

https://github.com/fukuchi/libqrencode/issues

Questions of license compliance are also welcome.


ACKNOWLEDGMENTS
===============
QR Code is registered trademarks of DENSO WAVE INCORPORATED in JAPAN and other
countries.

Reed-Solomon encoder is written by Phil Karn, KA9Q.
Copyright (C) 2002, 2003, 2004, 2006 Phil Karn, KA9Q

- NANKI Haruo           - improved lower-case characters encoding
- Philippe Delcroix     - improved mask evaluation
- Yusuke Mihara         - structured-append support
- David Dahl            - DPI and SVG support patch
- Adam Shepherd         - bug fix patch of the mask evaluation
- Josef Eisl (zapster)  - EPS support patch
- Colin (moshen)        - ANSI support patch
- Ralf Ertzinger        - ASCII support patch
- Yutaka Niibe (gniibe) - various bug fix patches
- Dan Storm (Repox)     - SVG support patch
- Lennart Poettering (mezcalero) - improved text art patch
- Yann Droneaud         - improved input validation patch
- Viona                 - bug fix patch for string splitting
- Daniel Dörrhöfer      - RLE option, some bug fixes, Travis configuration
- Greg Hart             - PNG32 support patch
- Shigeyuki Hirai, Paul Janssens, wangsai, Gavan Fantom, Matthew Baker, Rob Ryan, Fred Steinhaeuser, Terry Burton, chisj, vlad417, Petr, Hassan Hajji, Emmanuel Blot, ßlúèÇhîp, Heiko Becker, Gavin Andresen, David Binderman, ralgozino, Sean McMurray, win32asm, Tobias Klauser, Antenore, Yoshimichi Inoue, Sunil Maganally, Norman Gray, Danomi Manchego - bug report / suggestion
