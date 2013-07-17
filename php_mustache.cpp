
#include "php_mustache.hpp"



/* {{{ Declarations --------------------------------------------------------- */

ZEND_DECLARE_MODULE_GLOBALS(mustache)
static PHP_MINIT_FUNCTION(mustache);
static PHP_MSHUTDOWN_FUNCTION(mustache);
static PHP_MINFO_FUNCTION(mustache);
static PHP_GINIT_FUNCTION(mustache);

extern zend_class_entry * MustacheData_ce_ptr;
extern zend_class_entry * MustacheException_ce_ptr;
extern zend_class_entry * MustacheParserException_ce_ptr;
extern zend_class_entry * MustacheTemplate_ce_ptr;

extern PHP_MINIT_FUNCTION(mustache_data);
extern PHP_MINIT_FUNCTION(mustache_exceptions);
extern PHP_MINIT_FUNCTION(mustache_mustache);
extern PHP_MINIT_FUNCTION(mustache_template);

/* }}} ---------------------------------------------------------------------- */
/* {{{ Module Entry --------------------------------------------------------- */

zend_module_entry mustache_module_entry = {
  STANDARD_MODULE_HEADER,
  (char *) PHP_MUSTACHE_NAME,    /* Name */
  NULL,                          /* Functions */
  PHP_MINIT(mustache),           /* MINIT */
  NULL,                          /* MSHUTDOWN */
  NULL,                          /* RINIT */
  NULL,                          /* RSHUTDOWN */
  PHP_MINFO(mustache),           /* MINFO */
  (char *) PHP_MUSTACHE_VERSION, /* Version */
  PHP_MODULE_GLOBALS(mustache),  /* Globals */
  PHP_GINIT(mustache),           /* GINIT */
  NULL,
  NULL,
  0, 0, NULL, 0, (char *) ZEND_MODULE_BUILD_ID /* STANDARD_MODULE_PROPERTIES_EX */
};  

#ifdef COMPILE_DL_MUSTACHE 
extern "C" {
  ZEND_GET_MODULE(mustache)      // Common for all PHP extensions which are build as shared modules  
}
#endif

/* }}} ---------------------------------------------------------------------- */
/* {{{ INI Settings --------------------------------------------------------- */

PHP_INI_BEGIN()
  STD_PHP_INI_BOOLEAN((char *) "mustache.default_escape", (char *) "1", PHP_INI_ALL, OnUpdateBool, default_escape_by_default, zend_mustache_globals, mustache_globals)
  STD_PHP_INI_ENTRY((char *) "mustache.default_start", (char *) "{{", PHP_INI_ALL, OnUpdateString, default_start_sequence, zend_mustache_globals, mustache_globals)
  STD_PHP_INI_ENTRY((char *) "mustache.default_stop", (char *) "}}", PHP_INI_ALL, OnUpdateString, default_stop_sequence, zend_mustache_globals, mustache_globals)
PHP_INI_END()

/* }}} ---------------------------------------------------------------------- */
/* {{{ Module Hooks --------------------------------------------------------- */

static PHP_MINIT_FUNCTION(mustache)
{
  REGISTER_INI_ENTRIES();
  
  PHP_MINIT(mustache_mustache)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_data)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_template)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_exceptions)(INIT_FUNC_ARGS_PASSTHRU);
  
  return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(mustache)
{
  UNREGISTER_INI_ENTRIES();
  
  return SUCCESS;
}

static PHP_GINIT_FUNCTION(mustache)
{
  mustache_globals->default_escape_by_default = 1;
  mustache_globals->default_start_sequence = (char *) "{{";
  mustache_globals->default_stop_sequence = (char *) "}}";
}

static PHP_MINFO_FUNCTION(mustache)
{
  php_info_print_table_start();
  php_info_print_table_row(2, "Version", PHP_MUSTACHE_VERSION);
  php_info_print_table_row(2, "Released", PHP_MUSTACHE_RELEASE);
  php_info_print_table_row(2, "Revision", PHP_MUSTACHE_BUILD);
  php_info_print_table_row(2, "Authors", PHP_MUSTACHE_AUTHORS);
  php_info_print_table_row(2, "Spec Version", PHP_MUSTACHE_SPEC);
  php_info_print_table_row(2, "Libmustache Version", mustache_version());
#if HAVE_TCMALLOC
  php_info_print_table_row(2, "tcmalloc support", "enabled");
#else
  php_info_print_table_row(2, "tcmalloc support", "disabled");
#endif
#if HAVE_CXX11
  php_info_print_table_row(2, "c++11 unordered map support", "enabled");
#else
  php_info_print_table_row(2, "c++11 unordered map support", "disabled");
#endif
  php_info_print_table_end();
  
  DISPLAY_INI_ENTRIES();
}

/* }}} ---------------------------------------------------------------------- */
/* {{{ Utils ---------------------------------------------------------------- */

void mustache_exception_handler(TSRMLS_D)
{
#if PHP_MUSTACHE_THROW_EXCEPTIONS
  throw;
#else
  try {
    throw;
  } catch( mustache::TokenizerException& e ) {
    zval * exception = zend_throw_exception_ex(MustacheParserException_ce_ptr, 
            0 TSRMLS_CC, (char *) e.what(), "MustacheParserException");
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

/* }}} ---------------------------------------------------------------------- */
