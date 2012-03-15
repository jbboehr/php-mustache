
#ifndef PHP_MUSTACHE_TEMPLATE_HPP
#define PHP_MUSTACHE_TEMPLATE_HPP

#include "php_mustache.hpp"


static zend_class_entry * MustacheTemplate_ce_ptr = NULL;


typedef struct _php_obj_MustacheTemplate {
    zend_object obj;
    std::string * tmpl;
    mustache::Mustache * mustache;
    mustache::Node * node;
} php_obj_MustacheTemplate;


PHP_MINIT_FUNCTION(mustache_template);


PHP_METHOD(MustacheTemplate, __construct);
ZEND_BEGIN_ARG_INFO_EX(MustacheTemplate____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, vars)
ZEND_END_ARG_INFO()
        
PHP_METHOD(MustacheTemplate, render);
ZEND_BEGIN_ARG_INFO_EX(MustacheTemplate__render_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, vars)
    ZEND_ARG_INFO(0, partials)
ZEND_END_ARG_INFO()
        
PHP_METHOD(MustacheTemplate, toArray);
ZEND_BEGIN_ARG_INFO_EX(MustacheTemplate__toArray_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()
        
PHP_METHOD(MustacheTemplate, __toString);
ZEND_BEGIN_ARG_INFO_EX(MustacheTemplate____toString_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()


#endif
