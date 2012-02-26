
PHP_ARG_ENABLE(mustache,
    [Whether to enable the "mustache" extension]
    [  --enable-mustache         Enable mustache support])

if test "$PHP_MUSTACHE" != "no"; then
    PHP_REQUIRE_CXX()
    dnl export CC=g++

    PHP_SUBST(MUSTACHE_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, MUSTACHE_SHARED_LIBADD)
    PHP_NEW_EXTENSION(mustache,
        php_mustache.cpp php_mustache_methods.cpp mustache.cpp,
        $ext_shared)
fi
