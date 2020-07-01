
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_mustache.h"

#include <Zend/zend_closures.h>

#include "mustache/mustache.hpp"
#include "mustache_private.hpp"
#include "mustache_zend_closure_lambda.hpp"

ZendClosureLambda::~ZendClosureLambda()
{
  zval_ptr_dtor(closure);
}

int ZendClosureLambda::getUserFunctionParamCount()
{
  const zend_function * func = zend_get_closure_method_def(closure);
  return func->common.num_args;
}

int ZendClosureLambda::invokeUserFunction(zval *retval_ptr, int param_count, zval params[])
{
  return call_user_function_ex(CG(function_table), NULL, closure, retval_ptr, param_count, params, 1, NULL);
}
