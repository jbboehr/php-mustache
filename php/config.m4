
PHP_ARG_ENABLE(mustache,
    [Whether to enable the "mustache" extension]
    [  --enable-mustache         Enable mustache support])

PHP_ARG_ENABLE(mustache_profiler,
    [Whether to enable the "mustache" extension profiler]
    [  --enable-mustache-profiler         Enable mustache profiler support])

if test "$PHP_MUSTACHE" != "no"; then
    PHP_REQUIRE_CXX()
    dnl export CC=g++

    PHP_ADD_INCLUDE(../src)

    PHP_SUBST(MUSTACHE_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, MUSTACHE_SHARED_LIBADD)
    PHP_NEW_EXTENSION(mustache,
        php_mustache.cpp php_mustache_methods.cpp ../src/mustache.cpp,
        $ext_shared)
fi

if test "$PHP_MUSTACHE_PROFILER" != "no"; then
    AC_DEFINE(HAVE_MUSTACHE_PROFILER, 1, [Whether you want profiler])
fi
