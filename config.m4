
AC_CONFIG_MACRO_DIR([mustache/m4])

dnl FUNCTIONS ------------------------------------------------------------------
AC_DEFUN([PHP_MUSTACHE_ADD_SOURCES], [
  PHP_MUSTACHE_SOURCES="$PHP_MUSTACHE_SOURCES $1"
])
AC_DEFUN([PHP_MUSTACHE_ADD_FLAGS], [
  PHP_MUSTACHE_FLAGS="$PHP_MUSTACHE_FLAGS $1"
])

dnl C++11 ----------------------------------------------------------------------
dnl Switch the includes if compiling into PHP
m4_include(ax_cxx_compile_stdcxx_11.m4)
AX_CXX_COMPILE_STDCXX_11

dnl MUSTACHE -------------------------------------------------------------------
PHP_ARG_ENABLE(mustache, whether to enable mustache support,
dnl Make sure that the comment is aligned:
[  --enable-mustache Enable mustache support])

PHP_ARG_ENABLE(mustache-coverage, whether to enable mustache coverage support,
[AS_HELP_STRING([--enable-mustache-coverage], [Enable mustache coverage support])], [no], [no])

dnl LIBMUSTACHE ----------------------------------------------------------------
PHP_ARG_WITH(libmustache, libmustache location,
dnl Make sure that the comment is aligned:
[  --with-libmustache libmustache location (use pkg-config by default)])

SEARCH_PATH="/usr/local /usr"
SEARCH_FOR="/include/mustache/mustache.hpp"

AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

AC_MSG_CHECKING([for libmustache files])

dnl Priority to user provided path
if test -r $PHP_LIBMUSTACHE/$SEARCH_FOR; then
  LIBMUSTACHE_DIR=$PHP_LIBMUSTACHE
  AC_MSG_RESULT(found in $LIBMUSTACHE_DIR)

dnl Default to pkg-config output
elif test -x "$PKG_CONFIG" && $PKG_CONFIG --exists mustache; then
  LIBMUSTACHE_CFLAGS=`$PKG_CONFIG mustache --cflags`
  LIBMUSTACHE_LIBS=`$PKG_CONFIG mustache --libs`
  LIBMUSTACHE_VERSION=`$PKG_CONFIG mustache --modversion`
  AC_MSG_RESULT(version $LIBMUSTACHE_VERSION found using pkg-config)
  PHP_EVAL_LIBLINE($LIBMUSTACHE_LIBS, MUSTACHE_SHARED_LIBADD)
  PHP_EVAL_INCLINE($LIBMUSTACHE_CFLAGS)

dnl Fallback to some well known locations
else
  for i in $SEARCH_PATH ; do
    if test -r $i/$SEARCH_FOR; then
      LIBMUSTACHE_DIR=$i
      AC_MSG_RESULT(found in $i)
      break
    fi
  done
  if test "x$LIBMUSTACHE_DIR" == "x"; then
    AC_MSG_ERROR([libmustache not found])
  fi
fi

dnl MAIN -----------------------------------------------------------------------
if test "$PHP_MUSTACHE" != "no"; then
  AH_BOTTOM([
#ifdef __clang__
#include "main/php_config.h"
#/**/undef/**/ HAVE_ASM_GOTO
#endif
  ])
  if test "$PHP_MUSTACHE_COVERAGE" == "yes"; then
    CXXFLAGS="--coverage -fprofile-arcs -ftest-coverage $CXXFLAGS"
    LDFLAGS="--coverage -lgcov $LDFLAGS"
  fi

  AC_DEFINE(HAVE_MUSTACHE, 1, [Whether you have mustache support])
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, MUSTACHE_SHARED_LIBADD)

  PHP_MUSTACHE_ADD_SOURCES([
    php_mustache.cpp
    mustache_ast.cpp
    mustache_mustache.cpp
    mustache_exceptions.cpp
    mustache_data.cpp
    mustache_template.cpp
    mustache_lambda.cpp
    mustache_lambda_helper.cpp
    mustache_class_method_lambda.cpp
    mustache_zend_closure_lambda.cpp
  ])

  AC_DEFINE(HAVE_MUSTACHELIB,1,[ ])
  if test -n "$LIBMUSTACHE_DIR"; then
    PHP_ADD_LIBRARY_WITH_PATH(mustache, $LIBMUSTACHE_DIR/$PHP_LIBDIR, MUSTACHE_SHARED_LIBADD)
    PHP_ADD_INCLUDE($LIBMUSTACHE_DIR/include)
  fi

  PHP_SUBST(MUSTACHE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mustache, $PHP_MUSTACHE_SOURCES, $ext_shared, , $PHP_MUSTACHE_FLAGS, cxx)
fi

