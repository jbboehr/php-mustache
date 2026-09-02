
AC_CONFIG_MACRO_DIR([mustache/m4])

dnl FUNCTIONS ------------------------------------------------------------------
AC_DEFUN([PHP_MUSTACHE_ADD_SOURCES], [
  PHP_MUSTACHE_SOURCES="$PHP_MUSTACHE_SOURCES $1"
])
AC_DEFUN([PHP_MUSTACHE_ADD_FLAGS], [
  PHP_MUSTACHE_FLAGS="$PHP_MUSTACHE_FLAGS $1"
])

dnl MUSTACHE -------------------------------------------------------------------
PHP_ARG_ENABLE(mustache, whether to enable mustache support,
dnl Make sure that the comment is aligned:
[  --enable-mustache Enable mustache support])

PHP_ARG_ENABLE(mustache-coverage, whether to enable mustache coverage support,
[AS_HELP_STRING([--enable-mustache-coverage], [Enable mustache coverage support])], [no], [no])

PHP_ARG_ENABLE(mustache-sanitizers, whether to enable mustache sanitizer support,
[AS_HELP_STRING([--enable-mustache-sanitizers], [Enable AddressSanitizer and UndefinedBehaviorSanitizer support (requires a sanitizer-instrumented PHP; see README)])], [no], [no])

PHP_ARG_ENABLE(mustache-archive-benchmark, whether to enable the archived-template benchmark bridge,
[AS_HELP_STRING([--enable-mustache-archive-benchmark], [Enable the experimental archived-template benchmark bridge])], [no], [no])

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
  LIBMUSTACHE_CFLAGS="-I$LIBMUSTACHE_DIR/include"
  AC_MSG_RESULT(found in $LIBMUSTACHE_DIR)

dnl Default to pkg-config output
elif test -x "$PKG_CONFIG" && $PKG_CONFIG --exists 'mustache >= 0.6.0'; then
  LIBMUSTACHE_CFLAGS=`$PKG_CONFIG mustache --cflags`
  LIBMUSTACHE_LIBS=`$PKG_CONFIG mustache --libs`
  LIBMUSTACHE_VERSION=`$PKG_CONFIG mustache --modversion`
  AC_MSG_RESULT(version $LIBMUSTACHE_VERSION found using pkg-config)
  PHP_EVAL_LIBLINE($LIBMUSTACHE_LIBS, MUSTACHE_SHARED_LIBADD)
  PHP_EVAL_INCLINE($LIBMUSTACHE_CFLAGS)
  PHP_MUSTACHE_ADD_FLAGS([$LIBMUSTACHE_CFLAGS])

elif test -x "$PKG_CONFIG" && $PKG_CONFIG --exists mustache; then
  LIBMUSTACHE_VERSION=`$PKG_CONFIG mustache --modversion`
  AC_MSG_ERROR([libmustache >= 0.6.0 required; version $LIBMUSTACHE_VERSION found])

dnl Fallback to some well known locations
else
  for i in $SEARCH_PATH ; do
    if test -r $i/$SEARCH_FOR; then
      LIBMUSTACHE_DIR=$i
      LIBMUSTACHE_CFLAGS="-I$LIBMUSTACHE_DIR/include"
      AC_MSG_RESULT(found in $i)
      break
    fi
  done
  if test "x$LIBMUSTACHE_DIR" = "x"; then
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
  if test "$PHP_MUSTACHE_COVERAGE" = "yes"; then
    CXXFLAGS="--coverage -fprofile-arcs -ftest-coverage $CXXFLAGS"
    LDFLAGS="--coverage -lgcov $LDFLAGS"
  fi
  if test "$PHP_MUSTACHE_SANITIZERS" = "yes"; then
    CXXFLAGS="$CXXFLAGS -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=all"
    LDFLAGS="$LDFLAGS -fsanitize=address,undefined -fno-sanitize-recover=all"
  fi

  AC_DEFINE(HAVE_MUSTACHE, 1, [Whether you have mustache support])
  PHP_REQUIRE_CXX()
  PHP_CXX_COMPILE_STDCXX(17, mandatory, PHP_MUSTACHE_STDCXX)
  PHP_MUSTACHE_ADD_FLAGS([$PHP_MUSTACHE_STDCXX])
  PHP_ADD_LIBRARY(stdc++, 1, MUSTACHE_SHARED_LIBADD)

  AC_LANG_PUSH([C++])
  PHP_MUSTACHE_SAVED_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS $PHP_MUSTACHE_STDCXX $LIBMUSTACHE_CFLAGS"
  AC_MSG_CHECKING([whether libmustache provides the C++17 ABI 6 contract])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <mustache/mustache_config.h>
#include <string_view>

#if !defined(MUSTACHE_CXX_STANDARD) || MUSTACHE_CXX_STANDARD < 17
# error libmustache 0.6.0 or later is required
#endif
  ]], [[
    std::string_view value;
    return value.empty() ? 0 : 1;
  ]])], [
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([libmustache >= 0.6.0 and a C++17 compiler are required])
  ])

  if test "$PHP_MUSTACHE_ARCHIVE_BENCHMARK" = "yes"; then
    AC_MSG_CHECKING([whether libmustache provides archived templates])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <mustache/archived_template.hpp>

#if !defined(MUSTACHE_HAVE_ARCHIVED_TEMPLATES)
# error libmustache archived templates are unavailable
#endif
    ]], [[
      mustache::ArchivedTemplateLimits limits;
      return limits.maxArchiveBytes == 0;
    ]])], [
      AC_MSG_RESULT([yes])
      AC_DEFINE(PHP_MUSTACHE_ARCHIVE_BENCHMARK, 1,
        [Whether the experimental archived-template benchmark bridge is enabled])
    ], [
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([--enable-mustache-archive-benchmark requires libmustache archived-template support])
    ])
  fi
  CXXFLAGS="$PHP_MUSTACHE_SAVED_CXXFLAGS"
  AC_LANG_POP([C++])

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
