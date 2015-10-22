
#include "mustache_private.hpp"

ZendClosureLambda::~ZendClosureLambda()
{
#if PHP_MAJOR_VERSION >= 7
  zval_ptr_dtor(closure);
#else
  zval_ptr_dtor(&closure);
#endif
}

int ZendClosureLambda::getUserFunctionParamCount()
{
  TSRMLS_FETCH();

  const zend_function * func = zend_get_closure_method_def(closure TSRMLS_CC);
  return func->common.num_args;
}

#if PHP_MAJOR_VERSION >= 7
int ZendClosureLambda::invokeUserFunction(zval *retval_ptr, int param_count, zval params[])
{
  return call_user_function_ex(CG(function_table), NULL, closure, retval_ptr, param_count, params, 1, NULL);
}
#else
int ZendClosureLambda::invokeUserFunction(zval **retval_ptr_ptr, int param_count, zval **params[] TSRMLS_DC)
{
  return call_user_function_ex(CG(function_table), NULL, closure, retval_ptr_ptr, param_count, params, 1, NULL TSRMLS_CC);
}
#endif
