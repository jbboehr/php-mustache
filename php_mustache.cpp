
#include "mustache_private.hpp"

/* {{{ prototypes */
ZEND_DECLARE_MODULE_GLOBALS(mustache)
/* }}} */

/* {{{ php.ini directive registration */
PHP_INI_BEGIN()
  STD_PHP_INI_BOOLEAN((char *) "mustache.default_escape", (char *) "1", PHP_INI_ALL, OnUpdateBool, default_escape_by_default, zend_mustache_globals, mustache_globals)
  STD_PHP_INI_ENTRY((char *) "mustache.default_start", (char *) "{{", PHP_INI_ALL, OnUpdateString, default_start_sequence, zend_mustache_globals, mustache_globals)
  STD_PHP_INI_ENTRY((char *) "mustache.default_stop", (char *) "}}", PHP_INI_ALL, OnUpdateString, default_stop_sequence, zend_mustache_globals, mustache_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
static PHP_MINIT_FUNCTION(mustache)
{
  REGISTER_INI_ENTRIES();
  
  PHP_MINIT(mustache_ast)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_code)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_mustache)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_data)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_template)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_exceptions)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_lambda_helper)(INIT_FUNC_ARGS_PASSTHRU);
  
  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
static PHP_MSHUTDOWN_FUNCTION(mustache)
{
  UNREGISTER_INI_ENTRIES();
  
  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
static PHP_MINFO_FUNCTION(mustache)
{
  char opsize[3];
  snprintf(opsize, 3, "%d", _C_OP_SIZE);
  
  php_info_print_table_start();
  php_info_print_table_row(2, "Version", PHP_MUSTACHE_VERSION);
  php_info_print_table_row(2, "Released", PHP_MUSTACHE_RELEASE);
  php_info_print_table_row(2, "Revision", PHP_MUSTACHE_BUILD);
  php_info_print_table_row(2, "Authors", PHP_MUSTACHE_AUTHORS);
  php_info_print_table_row(2, "Spec Version", PHP_MUSTACHE_SPEC);
  php_info_print_table_row(2, "Libmustache Version", mustache_version());
  php_info_print_table_row(2, "Libmustache Operand Size", opsize);
#if MUSTACHE_HAVE_CXX11
  php_info_print_table_row(2, "c++11 unordered map support", "enabled");
#else
  php_info_print_table_row(2, "c++11 unordered map support", "disabled");
#endif
  php_info_print_table_end();
  
  DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(mustache)
{
  mustache_globals->default_escape_by_default = 1;
  mustache_globals->default_start_sequence = (char *) "{{";
  mustache_globals->default_stop_sequence = (char *) "}}";
}
/* }}} */

/* {{{ mustache_module_entry */
zend_module_entry mustache_module_entry = {
  STANDARD_MODULE_HEADER,
  (char *) PHP_MUSTACHE_NAME,    /* Name */
  NULL,                          /* Functions */
  PHP_MINIT(mustache),           /* MINIT */
  PHP_MSHUTDOWN(mustache),       /* MSHUTDOWN */
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
/* }}} */

#ifdef COMPILE_DL_MUSTACHE 
extern "C" {
  ZEND_GET_MODULE(mustache)      // Common for all PHP extensions which are build as shared modules  
}
#endif

