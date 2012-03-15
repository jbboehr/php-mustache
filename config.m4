
PHP_ARG_ENABLE(mustache, whether to enable mustache support,
dnl Make sure that the comment is aligned:
[ --enable-mustache Enable mustache support])

AC_DEFUN([PHP_MUSTACHE_ADD_SOURCES], [
  PHP_MUSTACHE_SOURCES="$PHP_MUSTACHE_SOURCES $1"
])

if test "$PHP_MUSTACHE" != "no"; then

  PHP_MUSTACHE_ADD_SOURCES([
    php_mustache.cpp
    mustache_mustache.cpp
    mustache_template.cpp
    mustache/mustache.cpp
    mustache/data.cpp
    mustache/node.cpp
    mustache/renderer.cpp
    mustache/tokenizer.cpp
    mustache/utils.cpp
  ])
  
  AC_DEFINE(HAVE_MUSTACHE, 1, [Whether you have mustache support]) 
  PHP_REQUIRE_CXX()
  
  PHP_ADD_INCLUDE(mustache)
  PHP_ADD_BUILD_DIR(mustache)

  PHP_ADD_LIBRARY(stdc++, 1, MUSTACHE_SHARED_LIBADD)
  PHP_SUBST(MUSTACHE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mustache, $PHP_MUSTACHE_SOURCES, $ext_shared)
fi
