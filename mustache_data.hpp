
#ifndef PHP_MUSTACHE_DATA_HPP
#define PHP_MUSTACHE_DATA_HPP


#include "php.h"


extern "C" zend_class_entry * MustacheData_ce_ptr;


typedef struct _php_obj_MustacheData {
    zend_object obj;
    mustache::Data * data;
} php_obj_MustacheData;


PHP_MINIT_FUNCTION(mustache_data);


PHP_METHOD(MustacheData, __construct);
ZEND_BEGIN_ARG_INFO_EX(MustacheData____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()
        
PHP_METHOD(MustacheData, toValue);
ZEND_BEGIN_ARG_INFO_EX(MustacheData__toValue_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()


#endif
