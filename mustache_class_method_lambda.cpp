
#include "php_mustache.h"
#include "mustache_private.hpp"
#include "mustache_class_method_lambda.hpp"

ClassMethodLambda::~ClassMethodLambda()
{
  zval_ptr_dtor(object);
  zval_dtor(&function_name);
}

int ClassMethodLambda::getUserFunctionParamCount()
{
  zend_class_entry * ce = Z_OBJCE_P(object);
  zval * zv = NULL;
  zend_function * function_entry = NULL;

  if( ce != NULL && &ce->function_table != NULL ) {
    zv = zend_hash_find(&ce->function_table, Z_STR(function_name));
    if( zv != NULL ) {
      function_entry = (zend_function *) Z_PTR_P(zv);
      return function_entry->common.num_args;
    }
  }

  return 0;
}

int ClassMethodLambda::invokeUserFunction(zval *retval_ptr, int param_count, zval params[])
{
  return call_user_function_ex(EG(function_table), object, &function_name, retval_ptr, param_count, params, 1, NULL);
}
