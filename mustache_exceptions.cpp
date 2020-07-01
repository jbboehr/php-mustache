
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_mustache.h"
#include <Zend/zend_exceptions.h>
#include "mustache/mustache.hpp"
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
    zend_class_entry * exception_ce = zend_exception_get_default();

    // MustacheException
    zend_class_entry mustache_exception_ce;
    INIT_CLASS_ENTRY(mustache_exception_ce, "MustacheException", NULL);
    MustacheException_ce_ptr = zend_register_internal_class_ex(&mustache_exception_ce, exception_ce);
    MustacheException_ce_ptr->create_object = exception_ce->create_object;

    // MustacheParserException
    zend_class_entry mustache_parser_exception_ce;
    INIT_CLASS_ENTRY(mustache_parser_exception_ce, "MustacheParserException", NULL);
    MustacheParserException_ce_ptr = zend_register_internal_class_ex(&mustache_parser_exception_ce, MustacheException_ce_ptr);
    MustacheParserException_ce_ptr->create_object = MustacheException_ce_ptr->create_object;

    return SUCCESS;
  } catch(...) {
    mustache_exception_handler();
    return FAILURE;
  }
}
/* }}} */

/* {{{ mustache_exception_handler */
void mustache_exception_handler()
{
#if PHP_MUSTACHE_THROW_EXCEPTIONS
  throw;
#else
  try {
    throw;
  } catch( mustache::TokenizerException& e ) {
    zval * exception;

    zval ex;
    zend_object * obj = zend_throw_exception_ex(MustacheParserException_ce_ptr,
            0, (char *) e.what(), "MustacheParserException");
    ZVAL_OBJ(&ex, obj);
    exception = &ex;

    zend_update_property_long(MustacheParserException_ce_ptr, exception,
            (char *) "templateLineNo", strlen("templateLineNo"), e.lineNo);
    zend_update_property_long(MustacheParserException_ce_ptr, exception,
            (char *) "templateCharNo", strlen("templateCharNo"), e.charNo);
  } catch( mustache::Exception& e ) {
    zend_throw_exception_ex(MustacheException_ce_ptr, 0,
            (char *) e.what(), "MustacheException");
    //php_error_docref(NULL, E_WARNING, e.what());
  } catch( InvalidParameterException& e ) {
    // @todo change this to an exception
    php_error_docref(NULL, E_WARNING, "%s", e.what());
  } catch( PhpInvalidParameterException& e ) {
    // The error message should be handled by PHP
  } catch( std::bad_alloc& e ) {
    php_error_docref(NULL, E_ERROR, "%s", "Memory allocation failed.");
  } catch( std::runtime_error& e ) {
    php_error_docref(NULL, E_ERROR, "%s", e.what());
  } catch(...) {
    php_error_docref(NULL, E_ERROR, "%s", "An unknown error has occurred.");
  }
#endif
}
/* }}} */

