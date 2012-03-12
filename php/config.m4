
PHP_ARG_ENABLE(mustache, whether to enable mustache support,
dnl Make sure that the comment is aligned:
[ --enable-mustache Enable mustache support])

AC_DEFUN([PHP_MUSTACHE_ADD_SOURCES], [
  PHP_MUSTACHE_SOURCES="$PHP_MUSTACHE_SOURCES $1"
])

if test "$PHP_MUSTACHE" != "no"; then

  for i in $PHP_MUSTACHE `readlink -f ..`; do
    if test -r $i/mustache.hpp; then
      MUSTACHE_DIR=$i
      AC_MSG_RESULT([found in $i])
      break
    fi
  done

  if test -z "$MUSTACHE_DIR" -o -z "$MUSTACHE_DIR"; then
     AC_MSG_RESULT([not found])
  fi

  PHP_MUSTACHE_ADD_SOURCES([
    php_mustache.cpp
    php_mustache_methods.cpp
    src/mustache.cpp
    src/data.cpp
    src/node.cpp
    src/renderer.cpp
    src/tokenizer.cpp
    src/utils.cpp
  ])
  
  AC_DEFINE(HAVE_MUSTACHE, 1, [Whether you have mustache support]) 
  PHP_REQUIRE_CXX()
  
  PHP_ADD_INCLUDE($MUSTACHE_DIR)
  PHP_ADD_BUILD_DIR($MUSTACHE_DIR)

  PHP_ADD_LIBRARY(stdc++, 1, MUSTACHE_SHARED_LIBADD)
  PHP_SUBST(MUSTACHE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mustache, $PHP_MUSTACHE_SOURCES, $ext_shared)
fi
