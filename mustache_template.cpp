
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_mustache.h"
#include "mustache_arginfo.h"
#include "mustache_exceptions.hpp"
#include "mustache_template.hpp"

/* {{{ ZE2 OO definitions */
zend_class_entry * MustacheTemplate_ce_ptr;
/* }}} */

/* {{{ MustacheTemplate_methods */
static zend_function_entry MustacheTemplate_methods[] = {
  PHP_ME(MustacheTemplate, __construct, arginfo_class_MustacheTemplate___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheTemplate, __toString, arginfo_class_MustacheTemplate___toString, ZEND_ACC_PUBLIC)
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

    zend_declare_property_null(MustacheTemplate_ce_ptr, ZEND_STRL("template"), ZEND_ACC_PROTECTED);

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
    size_t template_len = 0;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O|s!",
            &_this_zval, MustacheTemplate_ce_ptr, &template_str, &template_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();

    // Check if data was null
    if( template_len > 0 && template_str != NULL ) {
      zend_update_property_stringl(MustacheTemplate_ce_ptr, Z_OBJ_P(_this_zval), ZEND_STRL("template"), template_str, template_len);
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
    ZVAL_UNDEF(&rv);
    zval * value = zend_read_property(MustacheTemplate_ce_ptr, Z_OBJ_P(_this_zval), ZEND_STRL("template"), 1, &rv);
    if( UNEXPECTED(EG(exception) != NULL) ) {
      if( !Z_ISUNDEF(rv) ) {
        zval_ptr_dtor(&rv);
      }
      RETURN_THROWS();
    }
    if( value == &EG(uninitialized_zval) || Z_TYPE_P(value) == IS_UNDEF ) {
      if( !Z_ISUNDEF(rv) ) {
        zval_ptr_dtor(&rv);
      }
      RETURN_EMPTY_STRING();
    }

    zend_string * string_value = zval_try_get_string(value);
    if( !Z_ISUNDEF(rv) ) {
      zval_ptr_dtor(&rv);
    }
    if( UNEXPECTED(string_value == NULL) ) {
      RETURN_THROWS();
    }
    if( UNEXPECTED(EG(exception) != NULL) ) {
      zend_string_release(string_value);
      RETURN_THROWS();
    }
    RETURN_STR(string_value);

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheTemplate::__toString */
