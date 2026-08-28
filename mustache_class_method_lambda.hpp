
#ifndef PHP_MUSTACHE_CLASS_METHOD_LAMBDA_HPP
#define PHP_MUSTACHE_CLASS_METHOD_LAMBDA_HPP

#include "mustache_lambda.hpp"

class ClassMethodLambda : public Lambda {
  private:
    zval object;
    zval function_name;

  protected:
    int getUserFunctionParamCount();
    int invokeUserFunction(zval *retval_ptr, int param_count, zval params[]);

  public:
    ClassMethodLambda(zval * object_value, const char * function_name_string, size_t function_name_length) {
      ZVAL_COPY(&object, object_value);
      ZVAL_STRINGL(&function_name, function_name_string, function_name_length);
    };
    void addGcValues(zend_get_gc_buffer * gc_buffer) override;
    ClassMethodLambda(const ClassMethodLambda&) = delete;
    ClassMethodLambda& operator=(const ClassMethodLambda&) = delete;
    ClassMethodLambda(ClassMethodLambda&&) = delete;
    ClassMethodLambda& operator=(ClassMethodLambda&&) = delete;
    ~ClassMethodLambda();
};

#endif /* PHP_MUSTACHE_CLASS_METHOD_LAMBDA_HPP */
