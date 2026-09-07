
#ifndef PHP_MUSTACHE_MUSTACHE_HPP
#define PHP_MUSTACHE_MUSTACHE_HPP

#include "mustache/mustache.hpp"

struct php_obj_Mustache {
    mustache::Mustache * mustache;
    mustache::LambdaStringMode lambda_string_mode;
    // Negative values mean no aggregate limit. Initialized in create_object.
    zend_long max_partial_entries;
    zend_long max_partial_text_bytes;
    zend_object std;
};

php_obj_Mustache * php_mustache_mustache_object_fetch_object(zval * zv);

PHP_MINIT_FUNCTION(mustache_mustache);

mustache::Mustache * mustache_new_Mustache();

PHP_METHOD(Mustache, __construct);
PHP_METHOD(Mustache, getEscapeByDefault);
PHP_METHOD(Mustache, getLambdaStringMode);
PHP_METHOD(Mustache, getStartSequence);
PHP_METHOD(Mustache, getStopSequence);
PHP_METHOD(Mustache, setEscapeByDefault);
PHP_METHOD(Mustache, setLambdaStringMode);
PHP_METHOD(Mustache, setStartSequence);
PHP_METHOD(Mustache, setStopSequence);
PHP_METHOD(Mustache, setPartialLimits);
PHP_METHOD(Mustache, parse);
PHP_METHOD(Mustache, render);
PHP_METHOD(Mustache, tokenize);
PHP_METHOD(Mustache, debugDataStructure);

#if defined(PHP_MUSTACHE_ARCHIVE_BENCHMARK)
PHP_METHOD(Mustache, benchmarkSerializeArchive);
PHP_METHOD(Mustache, benchmarkRenderArchive);
#endif

#endif /* PHP_MUSTACHE_MUSTACHE_HPP */
