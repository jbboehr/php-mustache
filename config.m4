
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
dnl m4_include(ext/mustache/mustache/m4/ax_cxx_compile_stdcxx_11.m4)
m4_include(mustache/m4/ax_cxx_compile_stdcxx_11.m4)
m4_include(mustache/m4/stl_hash.m4)
AX_CXX_COMPILE_STDCXX_11
AC_CXX_STL_HASH

dnl MUSTACHE -------------------------------------------------------------------
PHP_ARG_ENABLE(mustache, whether to enable mustache support,
dnl Make sure that the comment is aligned:
[  --enable-mustache Enable mustache support])

dnl TCMALLOC -------------------------------------------------------------------
AC_MSG_CHECKING(whether tcmalloc support is present and requested)
AC_ARG_ENABLE(mustache-tcmalloc,
[  --enable-mustache-tcmalloc     Enable mustache tcmalloc], 
[
  PHP_MUSTACHE_TCMALLOC=$enableval
  AC_MSG_RESULT($enableval)
], 
[
  PHP_MUSTACHE_TCMALLOC=no
  AC_MSG_RESULT(no)
])

if test "$PHP_MUSTACHE_TCMALLOC" == "yes"; then
  PHP_ADD_LIBRARY(tcmalloc, 1, MUSTACHE_SHARED_LIBADD)
  AC_DEFINE(HAVE_TCMALLOC, 1, [Whether tcmalloc support is present and requested])
fi

dnl LIBMUSTACHE ----------------------------------------------------------------
PHP_ARG_WITH(libmustache, libmustache location,
dnl Make sure that the comment is aligned:
[  --with-libmustache libmustache location])

SEARCH_PATH="/usr/local /usr"
SEARCH_FOR="/include/mustache/mustache.hpp"
AC_MSG_CHECKING([for libmustache files])
if test -r $PHP_LIBMUSTACHE/$SEARCH_FOR; then
  LIBMUSTACHE_DIR=$PHP_LIBMUSTACHE
  AC_MSG_RESULT(found in $LIBMUSTACHE_DIR)
elif test -r "./mustache/src"; then
  LIBMUSTACHE_DIR="./mustache/src"
  LIBMUSTACHE_DIR_IS_SOURCE=yes
  AC_MSG_RESULT(found source in $LIBMUSTACHE_DIR)
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
  
  AC_DEFINE(HAVE_MUSTACHE, 1, [Whether you have mustache support])
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, MUSTACHE_SHARED_LIBADD)
  
  PHP_MUSTACHE_ADD_SOURCES([
    php_mustache.cpp
    mustache_ast.cpp
    mustache_code.cpp
    mustache_mustache.cpp
    mustache_exceptions.cpp
    mustache_data.cpp
    mustache_template.cpp
  ])

  if test "x$LIBMUSTACHE_DIR_IS_SOURCE" == "xyes"; then
    AC_DEFINE(HAVE_MUSTACHESRC,1,[ ])
    PHP_ADD_INCLUDE(mustache/src)
    PHP_ADD_BUILD_DIR(mustache/src)
    PHP_MUSTACHE_ADD_SOURCES([
      mustache/src/mustache.cpp
      mustache/src/compiler.cpp
      mustache/src/data.cpp
      mustache/src/node.cpp
      mustache/src/renderer.cpp
      mustache/src/tokenizer.cpp
      mustache/src/utils.cpp
      mustache/src/vm.cpp
    ])
  else
    AC_DEFINE(HAVE_MUSTACHELIB,1,[ ])
    PHP_ADD_LIBRARY_WITH_PATH(mustache, $LIBMUSTACHE_DIR/lib, MUSTACHE_SHARED_LIBADD)
    PHP_ADD_INCLUDE($LIBMUSTACHE_DIR/include/mustache)
  fi
  
  PHP_SUBST(MUSTACHE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mustache, $PHP_MUSTACHE_SOURCES, $ext_shared, , $PHP_MUSTACHE_FLAGS)
fi
