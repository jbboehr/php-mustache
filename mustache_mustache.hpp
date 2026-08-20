
#ifndef PHP_MUSTACHE_MUSTACHE_HPP
#define PHP_MUSTACHE_MUSTACHE_HPP

#include "mustache/mustache.hpp"

#ifdef __cplusplus
   extern "C" {
#endif

struct php_obj_Mustache {
    mustache::Mustache * mustache;
    zend_object std;
};

php_obj_Mustache * php_mustache_mustache_object_fetch_object(zval * zv);

PHP_MINIT_FUNCTION(mustache_mustache);

mustache::Mustache * mustache_new_Mustache();
bool mustache_parse_data_param(zval * data, mustache::Mustache * mustache,
    mustache::Data ** node);

PHP_METHOD(Mustache, __construct);
PHP_METHOD(Mustache, getEscapeByDefault);
PHP_METHOD(Mustache, getStartSequence);
PHP_METHOD(Mustache, getStopSequence);
PHP_METHOD(Mustache, setEscapeByDefault);
PHP_METHOD(Mustache, setStartSequence);
PHP_METHOD(Mustache, setStopSequence);
PHP_METHOD(Mustache, parse);
PHP_METHOD(Mustache, render);
PHP_METHOD(Mustache, tokenize);
PHP_METHOD(Mustache, debugDataStructure);

#ifdef __cplusplus
  } // extern "C" 
#endif

#endif /* PHP_MUSTACHE_MUSTACHE_HPP */
