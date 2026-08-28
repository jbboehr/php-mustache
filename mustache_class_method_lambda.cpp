
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_mustache.h"
#include "mustache_private.hpp"
#include "mustache_class_method_lambda.hpp"

ClassMethodLambda::~ClassMethodLambda()
{
  zval_ptr_dtor(&object);
  zval_dtor(&function_name);
}

int ClassMethodLambda::getUserFunctionParamCount()
{
  zend_class_entry * ce = Z_OBJCE(object);
  zend_function * function_entry = NULL;

  if( ce != NULL ) {
    function_entry = (zend_function *) zend_hash_find_ptr_lc(
        &ce->function_table, Z_STR(function_name));
    if( function_entry != NULL ) {
      return function_entry->common.num_args;
    }
  }

  return 0;
}

int ClassMethodLambda::invokeUserFunction(zval *retval_ptr, int param_count, zval params[])
{
  return call_user_function(NULL, &object, &function_name, retval_ptr, param_count, params);
}
