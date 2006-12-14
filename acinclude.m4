dnl Test for libpng
AC_DEFUN([AM_PATH_LIBPNG],
[
  if test x$with_libpng != xno && test -z "$LIBPNG"; then
    AC_MSG_CHECKING(for libpng12)
    if pkg-config --exists libpng12 ; then
        AC_MSG_RESULT(yes)
        PNG='png'
        PNG_DEP_CFLAGS_PACKAGES=libpng12
        LIBPNG=`pkg-config --libs libpng12`
    else
      AC_MSG_RESULT(no)
      AC_CHECK_LIB(png, png_read_info,
        [AC_CHECK_HEADER(png.h,
          png_ok=yes,
          png_ok=no)],
        AC_MSG_WARN(*** PNG library not found), -lz -lm)
      if test "$png_ok" = yes; then
        AC_MSG_CHECKING([for png_structp in png.h])
        AC_TRY_COMPILE([#include <png.h>],
          [png_structp pp; png_infop info; png_colorp cmap; png_create_read_struct;],
          png_ok=yes,
          png_ok=no)
        AC_MSG_RESULT($png_ok)
        if test "$png_ok" = yes; then
          PNG='png'; LIBPNG='-lpng -lz'
        else
          AC_MSG_WARN(*** PNG library is too old)
        fi
      else
       AC_MSG_WARN(*** PNG header file not found)
      fi
    fi
  fi

  if test x$with_libpng != xno && test -z "$LIBPNG"; then
     AC_MSG_ERROR([
*** Checks for PNG library failed. You can build the library without it by
*** passing --without-tools to configure but utility tools are not to be
*** built.])
  fi

  AC_SUBST(LIBPNG)
])
