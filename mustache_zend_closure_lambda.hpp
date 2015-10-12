
#ifndef PHP_MUSTACHE_ZEND_CLOSURE_LAMBDA_HPP
#define PHP_MUSTACHE_ZEND_CLOSURE_LAMBDA_HPP

#include <Zend/zend_closures.h>

#include "mustache_lambda.hpp"

#ifdef __cplusplus
   extern "C" {
#endif

class ZendClosureLambda : public Lambda {
  private:
    zval * closure;

  protected:
    int getUserFunctionParamCount();
#if PHP_MAJOR_VERSION >= 7
    int invokeUserFunction(zval *retval_ptr, int param_count, zval params[]);
#else
    int invokeUserFunction(zval **retval_ptr_ptr, int param_count, zval **params[] TSRMLS_DC);
#endif

  public:
    ZendClosureLambda(zval * closure) : closure(closure) {
      Z_ADDREF_P(closure);
    };
    ~ZendClosureLambda();
};

#ifdef __cplusplus
  } // extern "C"
#endif

#endif /* PHP_MUSTACHE_ZEND_CLOSURE_LAMBDA_HPP */

