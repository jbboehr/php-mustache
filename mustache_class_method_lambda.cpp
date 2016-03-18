
#include "mustache_private.hpp"

ClassMethodLambda::~ClassMethodLambda()
{
#if PHP_MAJOR_VERSION >= 7
  zval_ptr_dtor(object);
#else
  zval_ptr_dtor(&object);
#endif
  zval_dtor(&function_name);
}

#if PHP_MAJOR_VERSION >= 7
int ClassMethodLambda::getUserFunctionParamCount()
{
  zend_class_entry * ce = Z_OBJCE_P(object);
  HashTable * function_table = NULL;
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
#else
int ClassMethodLambda::getUserFunctionParamCount()
{
  TSRMLS_FETCH();

  zend_class_entry * ce = Z_OBJCE_P(object);
  HashTable * function_table = NULL;
  zend_function * function_entry = NULL;

  if( ce != NULL &&
      &ce->function_table != NULL &&
      zend_hash_find(&ce->function_table, Z_STRVAL(function_name), Z_STRLEN(function_name) + 1, (void **) &function_entry) == SUCCESS ) {
    return function_entry->common.num_args;
  }

  return 0;
}
#endif

#if PHP_MAJOR_VERSION >= 7
int ClassMethodLambda::invokeUserFunction(zval *retval_ptr, int param_count, zval params[])
{
  return call_user_function_ex(EG(function_table), object, &function_name, retval_ptr, param_count, params, 1, NULL);
}
#else
int ClassMethodLambda::invokeUserFunction(zval **retval_ptr_ptr, int param_count, zval **params[] TSRMLS_DC)
{
  return call_user_function_ex(EG(function_table), &object, &function_name, retval_ptr_ptr, param_count, params, 1, NULL TSRMLS_CC);
}
#endif
