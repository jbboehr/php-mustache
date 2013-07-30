
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
AX_CXX_COMPILE_STDCXX_11()

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

dnl MAIN -----------------------------------------------------------------------
if test "$PHP_MUSTACHE" != "no"; then
  
  AC_DEFINE(HAVE_MUSTACHE, 1, [Whether you have mustache support])
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, MUSTACHE_SHARED_LIBADD)
  
  dnl if test "$ax_cv_cxx_compile_cxx0x_cxx" = yes; then
  dnl   PHP_MUSTACHE_ADD_FLAGS("-std=c++0x")
  dnl elif test "$ax_cv_cxx_compile_cxx0x_gxx" = yes; then
  dnl   PHP_MUSTACHE_ADD_FLAGS("-std=gnu++0x")
  dnl fi

  PHP_ADD_INCLUDE(mustache/src)
  PHP_ADD_BUILD_DIR(mustache/src)
  PHP_MUSTACHE_ADD_SOURCES([
    php_mustache.cpp
    mustache_ast.cpp
    mustache_code.cpp
    mustache_mustache.cpp
    mustache_exceptions.cpp
    mustache_data.cpp
    mustache_template.cpp
    mustache/src/mustache.cpp
    mustache/src/compiler.cpp
    mustache/src/data.cpp
    mustache/src/node.cpp
    mustache/src/renderer.cpp
    mustache/src/tokenizer.cpp
    mustache/src/utils.cpp
    mustache/src/vm.cpp
  ])
  
  PHP_SUBST(MUSTACHE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mustache, $PHP_MUSTACHE_SOURCES, $ext_shared, , $PHP_MUSTACHE_FLAGS)
fi
