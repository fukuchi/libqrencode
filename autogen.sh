#!/bin/sh

set -e

if [ -d /usr/local/share/aclocal ]; then
    ACLOCAL_DIR=/usr/local/share/aclocal
else if [ -d /usr/share/aclocal ]; then
    ACLOCAL_DIR=/usr/share/aclocal
    fi
fi

if [ ! -d use ]; then
    mkdir use
fi

aclocal -I $ACLOCAL_DIR

libtoolize --automake #--copy
automake --add-missing #--copy

autoconf
