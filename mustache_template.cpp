
#include "php_mustache.h"
#include "mustache_private.hpp"
#include "mustache_exceptions.hpp"
#include "mustache_template.hpp"

/* {{{ ZE2 OO definitions */
zend_class_entry * MustacheTemplate_ce_ptr;
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(MustacheTemplate____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheTemplate____toString_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ MustacheTemplate_methods */
static zend_function_entry MustacheTemplate_methods[] = {
  PHP_ME(MustacheTemplate, __construct, MustacheTemplate____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheTemplate, __toString, MustacheTemplate____toString_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mustache_template)
{
  try {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "MustacheTemplate", MustacheTemplate_methods);
    MustacheTemplate_ce_ptr = zend_register_internal_class(&ce);
    
    zend_declare_property_null(MustacheTemplate_ce_ptr, "template", sizeof("template") - 1, ZEND_ACC_PROTECTED);
    
    return SUCCESS;
  } catch(...) {
    mustache_exception_handler();
    return FAILURE;
  }
}
/* }}} */

/* {{{ proto void MustacheTemplate::__construct(string tmpl) */
PHP_METHOD(MustacheTemplate, __construct)
{
  try {
    // Custom parameters
    char * template_str = NULL;
    long template_len = 0;
    
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O|s",
            &_this_zval, MustacheTemplate_ce_ptr, &template_str, &template_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    
    // Check if data was null
    if( template_len > 0 && template_str != NULL ) {
      zend_update_property_stringl(MustacheTemplate_ce_ptr, _this_zval, 
            "template", sizeof("template") - 1, template_str, template_len);
    }
    
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheTemplate::__construct */

/* {{{ proto string MustacheTemplate::__toString() */
PHP_METHOD(MustacheTemplate, __toString)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, MustacheTemplate_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    
    // Return
    zval rv;
    zval * value = zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, "template", sizeof("template")-1, 1, &rv);
    convert_to_string(value);
    RETURN_ZVAL(value, 1, 0);
    
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheTemplate::__toString */

