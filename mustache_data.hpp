
#ifndef PHP_MUSTACHE_DATA_HPP
#define PHP_MUSTACHE_DATA_HPP

#include <mustache/mustache.hpp>

#ifdef __cplusplus
   extern "C" {
#endif

struct php_obj_MustacheData {
    mustache::Data * data;
    zend_object std;
};

extern zend_class_entry * MustacheData_ce_ptr;

struct php_obj_MustacheData * php_mustache_data_object_fetch_object(zval * zv);

PHP_MINIT_FUNCTION(mustache_data);

void mustache_data_from_zval(mustache::Data * node, zval * current);
void mustache_data_to_zval(mustache::Data * node, zval * current);

PHP_METHOD(MustacheData, __construct);
PHP_METHOD(MustacheData, toValue);

#ifdef __cplusplus
  } // extern "C" 
#endif

#endif /* PHP_MUSTACHE_DATA_HPP */

