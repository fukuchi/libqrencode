#!/bin/sh

set -e

if [ `uname -s` = Darwin ]; then
    LIBTOOLIZE=glibtoolize
else
    LIBTOOLIZE=libtoolize
fi

if [ -d /usr/local/share/aclocal ]; then
    ACLOCAL_DIR=/usr/local/share/aclocal
elif [ -d /opt/local/share/aclocal ]; then
    ACLOCAL_DIR=/opt/local/share/aclocal
elif [ -d /usr/share/aclocal ]; then
    ACLOCAL_DIR=/usr/share/aclocal
fi

if [ ! -d use ]; then
    mkdir use
fi

autoheader

aclocal -I $ACLOCAL_DIR

$LIBTOOLIZE --automake --copy
automake --add-missing --copy

autoconf
