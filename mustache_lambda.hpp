
#ifndef PHP_MUSTACHE_LAMBDA_HPP
#define PHP_MUSTACHE_LAMBDA_HPP

#include <mustache/lambda.hpp>
#include <string_view>

class Lambda : public mustache::Lambda {
  protected:
    virtual int getUserFunctionParamCount() = 0;
    virtual int invokeUserFunction(zval *retval_ptr, int param_count, zval params[]) = 0;
    std::string invokeUserFunctionAsString(int param_count, zval params[]);
  public:
    virtual void addGcValues(zend_get_gc_buffer * gc_buffer) = 0;
    std::string invoke() override;
    std::string invoke(
        std::string_view text, mustache::LambdaRenderContext context) override;
};

#endif /* PHP_MUSTACHE_LAMBDA_HPP */
