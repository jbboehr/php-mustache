
#ifndef PHP_MUSTACHE_DATA_HPP
#define PHP_MUSTACHE_DATA_HPP

#include <mustache/mustache.hpp>

#ifdef __cplusplus
   extern "C" {
#endif

#if PHP_MAJOR_VERSION < 7
struct php_obj_MustacheData {
    zend_object std;
    mustache::Data * data;
};
#else
struct php_obj_MustacheData {
    mustache::Data * data;
    zend_object std;
};
#endif

extern zend_class_entry * MustacheData_ce_ptr;

struct php_obj_MustacheData * php_mustache_data_object_fetch_object(zval * zv TSRMLS_DC);

PHP_MINIT_FUNCTION(mustache_data);

void mustache_data_from_zval(mustache::Data * node, zval * current TSRMLS_DC);
void mustache_data_to_zval(mustache::Data * node, zval * current TSRMLS_DC);

PHP_METHOD(MustacheData, __construct);
PHP_METHOD(MustacheData, toValue);

#ifdef __cplusplus
  } // extern "C" 
#endif

#endif /* PHP_MUSTACHE_DATA_HPP */

