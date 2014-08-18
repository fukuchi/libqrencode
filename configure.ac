m4_define([__MAJOR_VERSION], [3])dnl
m4_define([__MINOR_VERSION], [4])dnl
m4_define([__MICRO_VERSION], [4])dnl
m4_define([__VERSION], [__MAJOR_VERSION.__MINOR_VERSION.__MICRO_VERSION])dnl
AC_INIT(QRencode, __VERSION)

MAJOR_VERSION=__MAJOR_VERSION
MINOR_VERSION=__MINOR_VERSION
MICRO_VERSION=__MICRO_VERSION
AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(MICRO_VERSION)
AC_DEFINE_UNQUOTED([MAJOR_VERSION], [$MAJOR_VERSION], [Major version number])
AC_DEFINE_UNQUOTED([MINOR_VERSION], [$MINOR_VERSION], [Minor version number])
AC_DEFINE_UNQUOTED([MICRO_VERSION], [$MICRO_VERSION], [Micro version number])

AC_CONFIG_SRCDIR([qrencode.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR(use)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_DISABLE_STATIC
AC_C_CONST
AC_C_INLINE
AC_HEADER_STDC

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_RANLIB
PKG_PROG_PKG_CONFIG

AC_CONFIG_FILES([Makefile libqrencode.pc tests/Makefile qrencode.spec qrencode.1])

AC_CHECK_FUNCS([strdup])

dnl --enable-thread-safety
AC_ARG_ENABLE([thread-safety], [AS_HELP_STRING([--enable-thread-safety], [make the library thread-safe. [default=yes]])],
 [], [enable_thread_safety=yes])

if test x$enable_thread_safety = xyes; then
	AC_CHECK_LIB([pthread], [pthread_mutex_init], [AC_SUBST([LIBPTHREAD], [-lpthread])])
fi
AM_CONDITIONAL([HAVE_LIBPTHREAD], [test "x$ac_cv_lib_pthread_pthread_mutex_init" = "xyes" ])
# FIXME: isn't it able to integrate the followings to AC_CHECK_LIB?
if test x$ac_cv_lib_pthread_pthread_mutex_init = xyes ; then
	AC_DEFINE([HAVE_LIBPTHREAD], [1], [Define to 1 if using pthread is enabled.])
fi


dnl --with-tools
AC_ARG_WITH([tools], [AS_HELP_STRING([--with-tools], [build utility tools [default=yes]])],
 [build_tools=$withval], [build_tools=yes])

AM_CONDITIONAL(BUILD_TOOLS, [test "x$build_tools" = "xyes" ])
if test x$build_tools = xyes ; then
	PKG_CHECK_MODULES(png, "libpng")
fi

dnl --with-tests
AC_ARG_WITH([tests], [AS_HELP_STRING([--with-tests], [build tests [default=no]])],
 [build_tests=$withval], [build_tests=no])

AM_CONDITIONAL(BUILD_TESTS, [test "x$build_tests" = "xyes" ])
AH_VERBATIM([tests],
[/* Define to 'static' if no test programs will be compiled. */
#define __STATIC static
#undef WITH_TESTS
   ])
if test x$build_tests = xyes ; then
echo "#define __STATIC" >>confdefs.h
echo "#define WITH_TESTS 1" >>confdefs.h
else
echo "#define __STATIC static" >>confdefs.h
echo "/* #undef WITH_TESTS */" >>confdefs.h
fi

if test x$build_tests = xyes ; then
	SDL_REQUIRED_VERSION=1.2.0
	AM_PATH_SDL($SDL_REQUIRED_VERSION,,AC_MSG_WARN([*** SDL $SDL_REQUIRED_VERSION or better is required.]))
	AC_MSG_NOTICE([SDL check done.])
	AM_ICONV_LINK
fi
AM_CONDITIONAL(HAVE_SDL, [test "x$SDL_CFLAGS" != "x" ])


dnl --enable-gprof
AC_ARG_ENABLE([gprof], [AS_HELP_STRING([--enable-gprof], [generate extra code to write profile information suitable for gprof [default=no]])],
 [], [enable_gprof=no])

if test x$enable_gprof = xyes; then
	CFLAGS="$CFLAGS -g -pg"
fi


dnl --enable-gcov
AC_ARG_ENABLE([gcov], [AS_HELP_STRING([--enable-gcov], [generate extra code to write coverage information suitable for gcov [default=no]])],
 [], [enable_gcov=no])

if test x$enable_gcov = xyes; then
	CFLAGS="$CFLAGS --coverage"
fi


dnl --enable-mudflap
AC_ARG_ENABLE([mudflap], [AS_HELP_STRING([--enable-mudflap], [generate extra code to check memory leaks [default=no]])],
 [], [enable_mudflap=no])

if test x$enable_mudflap = xyes; then
	if test x$enable_thread_safety = xyes; then
		CFLAGS="$CFLAGS -fmudflapth"
		LDFLAGS="$LDFLAGS -lmudflapth"
	else
		CFLAGS="$CFLAGS -fmudflap"
		LDFLAGS="$LDFLAGS -lmudflap"
	fi
fi

dnl --enable-asan
AC_ARG_ENABLE([asan], [AS_HELP_STRING([--enable-asan], [use AddressSanitizer [default=no]])],
 [], [enable_asan=no])

if test x$enable_asan = xyes; then
	CFLAGS="$CFLAGS -fsanitize=address -fno-omit-frame-pointer"
	LDFLAGS="$LDFLAGS -fsanitize=address"
fi


dnl set CFLAGS
CFLAGS="-Wall $CFLAGS"


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
