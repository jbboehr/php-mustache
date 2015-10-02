
#ifndef PHP_MUSTACHE_MUSTACHE_HPP
#define PHP_MUSTACHE_MUSTACHE_HPP

#ifdef __cplusplus
   extern "C" {
#endif

#if PHP_MAJOR_VERSION < 7
struct php_obj_Mustache {
    zend_object std;
    mustache::Mustache * mustache;
};
#else
struct php_obj_Mustache {
    mustache::Mustache * mustache;
    zend_object std;
};
#endif

php_obj_Mustache * php_mustache_mustache_object_fetch_object(zval * zv TSRMLS_DC);

PHP_MINIT_FUNCTION(mustache_mustache);

void mustache_data_from_zval(mustache::Data * node, zval * current TSRMLS_DC);
bool mustache_parse_data_param(zval * data, mustache::Mustache * mustache,
    mustache::Data ** node TSRMLS_DC);
bool mustache_parse_partials_param(zval * array, mustache::Mustache * mustache,
    mustache::Node::Partials * partials TSRMLS_DC);
bool mustache_parse_template_param(zval * tmpl, mustache::Mustache * mustache,
    mustache::Node ** node TSRMLS_DC);

PHP_METHOD(Mustache, __construct);
PHP_METHOD(Mustache, getEscapeByDefault);
PHP_METHOD(Mustache, getStartSequence);
PHP_METHOD(Mustache, getStopSequence);
PHP_METHOD(Mustache, setEscapeByDefault);
PHP_METHOD(Mustache, setStartSequence);
PHP_METHOD(Mustache, setStopSequence);
PHP_METHOD(Mustache, compile);
PHP_METHOD(Mustache, execute);
PHP_METHOD(Mustache, parse);
PHP_METHOD(Mustache, render);
PHP_METHOD(Mustache, tokenize);
PHP_METHOD(Mustache, debugDataStructure);

#ifdef __cplusplus
  } // extern "C" 
#endif

#endif /* PHP_MUSTACHE_MUSTACHE_HPP */

