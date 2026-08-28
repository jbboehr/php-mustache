
#ifndef PHP_MUSTACHE_ZEND_CLOSURE_LAMBDA_HPP
#define PHP_MUSTACHE_ZEND_CLOSURE_LAMBDA_HPP

#include "mustache_lambda.hpp"

class ZendClosureLambda : public Lambda {
  private:
    zval closure;

  protected:
    int getUserFunctionParamCount();
    int invokeUserFunction(zval *retval_ptr, int param_count, zval params[]);

  public:
    ZendClosureLambda(zval * closure_value) {
      ZVAL_COPY(&closure, closure_value);
    };
    void addGcValues(zend_get_gc_buffer * gc_buffer) override;
    ZendClosureLambda(const ZendClosureLambda&) = delete;
    ZendClosureLambda& operator=(const ZendClosureLambda&) = delete;
    ZendClosureLambda(ZendClosureLambda&&) = delete;
    ZendClosureLambda& operator=(ZendClosureLambda&&) = delete;
    ~ZendClosureLambda();
};

#endif /* PHP_MUSTACHE_ZEND_CLOSURE_LAMBDA_HPP */
