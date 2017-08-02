
#include "php_mustache.h"
#include <Zend/zend_exceptions.h>
#include "php5to7.h"
#include "mustache_private.hpp"
#include "mustache_exceptions.hpp"

/* {{{ ZE2 OO definitions */
zend_class_entry * MustacheException_ce_ptr;
zend_class_entry * MustacheParserException_ce_ptr;
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mustache_exceptions)
{
  try {
    zend_class_entry * exception_ce = zend_exception_get_default(TSRMLS_C);
    
    // MustacheException
    zend_class_entry mustache_exception_ce;
    INIT_CLASS_ENTRY(mustache_exception_ce, "MustacheException", NULL);
    MustacheException_ce_ptr = _zend_register_internal_class_ex(&mustache_exception_ce, exception_ce);
    MustacheException_ce_ptr->create_object = exception_ce->create_object;
    
    // MustacheParserException
    zend_class_entry mustache_parser_exception_ce;
    INIT_CLASS_ENTRY(mustache_parser_exception_ce, "MustacheParserException", NULL);
    MustacheParserException_ce_ptr = _zend_register_internal_class_ex(&mustache_parser_exception_ce, MustacheException_ce_ptr);
    MustacheParserException_ce_ptr->create_object = MustacheException_ce_ptr->create_object;
    
    return SUCCESS;
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
    return FAILURE;
  }
}
/* }}} */

/* {{{ mustache_exception_handler */
void mustache_exception_handler(TSRMLS_D)
{
#if PHP_MUSTACHE_THROW_EXCEPTIONS
  throw;
#else
  try {
    throw;
  } catch( mustache::TokenizerException& e ) {
    zval * exception;
#if PHP_API_VERSION < 20131218
    exception = zend_throw_exception_ex(MustacheParserException_ce_ptr, 
            0 TSRMLS_CC, (char *) e.what(), "MustacheParserException");
#else
    zval ex;
    zend_object * obj = zend_throw_exception_ex(MustacheParserException_ce_ptr, 
            0 TSRMLS_CC, (char *) e.what(), "MustacheParserException");
    ZVAL_OBJ(&ex, obj);
    exception = &ex;
/*
	zend_update_property_long(base_exception_ce, &ex, "severity", sizeof("severity")-1, severity);
	return obj;
    zval exception;
    zend_object * exceptionobj = zend_throw_exception_ex(MustacheParserException_ce_ptr, 
            0 TSRMLS_CC, (char *) e.what(), "MustacheParserException");
    zval * exception = Z_OBJ_P(exceptionobj);
*/
#endif
	
    zend_update_property_long(MustacheParserException_ce_ptr, exception, 
            (char *) "templateLineNo", strlen("templateLineNo"), e.lineNo TSRMLS_CC);
    zend_update_property_long(MustacheParserException_ce_ptr, exception, 
            (char *) "templateCharNo", strlen("templateCharNo"), e.charNo TSRMLS_CC);
  } catch( mustache::Exception& e ) {
    zend_throw_exception_ex(MustacheException_ce_ptr, 0 TSRMLS_CC, 
            (char *) e.what(), "MustacheException");
    //php_error_docref(NULL TSRMLS_CC, E_WARNING, e.what());
  } catch( InvalidParameterException& e ) {
    // @todo change this to an exception
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", e.what());
  } catch( PhpInvalidParameterException& e ) {
    // The error message should be handled by PHP
  } catch( std::bad_alloc& e ) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "%s", "Memory allocation failed.");
  } catch( std::runtime_error& e ) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "%s", e.what());
  } catch(...) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "%s", "An unknown error has occurred.");
  }
#endif
}
/* }}} */

