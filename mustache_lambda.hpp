
#ifndef PHP_MUSTACHE_LAMBDA_HPP
#define PHP_MUSTACHE_LAMBDA_HPP

#include <mustache/lambda.hpp>

class Lambda : public mustache::Lambda {
  protected:
    virtual int getUserFunctionParamCount() = 0;
    virtual int invokeUserFunction(zval *retval_ptr, int param_count, zval params[]) = 0;
  public:
    std::string invoke();
    std::string invoke(std::string * text, mustache::Renderer * renderer);
};

#endif /* PHP_MUSTACHE_LAMBDA_HPP */

