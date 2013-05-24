
#include "php_mustache.hpp"
#include "mustache_exceptions.hpp"



// Class Entries  --------------------------------------------------------------

zend_class_entry * MustacheException_ce_ptr;
zend_class_entry * MustacheParserException_ce_ptr;



// MINIT -----------------------------------------------------------------------

PHP_MINIT_FUNCTION(mustache_exceptions)
{
  try {
    zend_class_entry * exception_ce = zend_exception_get_default(TSRMLS_C);
    
    // MustacheException
    zend_class_entry mustache_exception_ce;
    INIT_CLASS_ENTRY_EX(mustache_exception_ce, "MustacheException", strlen("MustacheException"), NULL);
    MustacheException_ce_ptr = zend_register_internal_class_ex(&mustache_exception_ce, 
            exception_ce, NULL TSRMLS_CC);
    MustacheException_ce_ptr->create_object = exception_ce->create_object;
    
    // MustacheParserException
    zend_class_entry mustache_parser_exception_ce;
    INIT_CLASS_ENTRY_EX(mustache_parser_exception_ce, "MustacheParserException", strlen("MustacheParserException"), NULL);
    MustacheParserException_ce_ptr = zend_register_internal_class_ex(&mustache_parser_exception_ce, 
            MustacheException_ce_ptr, NULL TSRMLS_CC);
    MustacheParserException_ce_ptr->create_object = MustacheException_ce_ptr->create_object;
    
    return SUCCESS;
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
    return FAILURE;
  }
}
