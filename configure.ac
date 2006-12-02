AC_INIT(QRencode)

MAJOR_VERSION=1
MINOR_VERSION=0
MICRO_VERSION=0
VERSION=$MAJOR_VERSION.$MINOR_VERSION.$MICRO_VERSION
AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(MICRO_VERSION)
AC_SUBST(VERSION)

AC_CONFIG_SRCDIR([qrencode.c])
AC_CONFIG_AUX_DIR(use)
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(qrencode, $VERSION)

AC_DISABLE_STATIC
AC_C_CONST
AC_C_INLINE
AC_HEADER_STDC

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL


SDL_REQUIRED_VERSION=1.2.0
AM_PATH_SDL($SDL_REQUIRED_VERSION,,AC_MSG_WARN([*** SDL $SDL_REQUIRED_VERSION or better is required.]))
AM_CONDITIONAL(HAVE_SDL, [test "x$SDL_CFLAGS" != "x" ])

CFLAGS="-Wall $CFLAGS"

AC_CONFIG_FILES([Makefile libqrencode.pc tests/Makefile])

AC_ARG_WITH([tools], [AC_HELP_STRING([--with-tools], [build utility tools [default=yes]])],
 [], [build_tools=yes])

if test x$build_tools = xyes ; then
	AM_PATH_LIBPNG
fi
AM_CONDITIONAL(BUILD_TOOLS, [test "x$build_tools" = "xyes" ])

AC_OUTPUT

echo ""
echo "Options used to compile and link:"
echo "  CC       = $CC"
echo "  CFLAGS   = $CFLAGS"
echo "  CPPFLAGS = $CPPFLAGS"
echo "  CXX      = $CXX"
echo "  CXXFLAGS = $CXXFLAGS"
echo "  LDFLAGS  = $LDFLAGS"
echo ""
