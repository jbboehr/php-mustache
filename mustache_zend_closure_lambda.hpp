
#ifndef PHP_MUSTACHE_ZEND_CLOSURE_LAMBDA_HPP
#define PHP_MUSTACHE_ZEND_CLOSURE_LAMBDA_HPP

#include <Zend/zend_closures.h>
#include "mustache_lambda.hpp"

class ZendClosureLambda : public Lambda {
  private:
    zval * closure;

  protected:
    int getUserFunctionParamCount();
    int invokeUserFunction(zval *retval_ptr, int param_count, zval params[]);

  public:
    ZendClosureLambda(zval * closure) : closure(closure) {
      Z_ADDREF_P(closure);
    };
    ~ZendClosureLambda();
};

#endif /* PHP_MUSTACHE_ZEND_CLOSURE_LAMBDA_HPP */

