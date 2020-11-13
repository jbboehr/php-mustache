
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
#if PHP_VERSION_ID < 80000
  const zend_function * func = zend_get_closure_method_def(closure);
#else
  const zend_function * func = zend_get_closure_method_def(Z_OBJ_P(closure));
#endif
  return func->common.num_args;
}

int ZendClosureLambda::invokeUserFunction(zval *retval_ptr, int param_count, zval params[])
{
  return call_user_function(NULL, NULL, closure, retval_ptr, param_count, params);
}
