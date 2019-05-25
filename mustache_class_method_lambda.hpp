
#ifndef PHP_MUSTACHE_CLASS_METHOD_LAMBDA_HPP
#define PHP_MUSTACHE_CLASS_METHOD_LAMBDA_HPP

#include "mustache_lambda.hpp"

#ifdef __cplusplus
   extern "C" {
#endif

class ClassMethodLambda : public Lambda {
  private:
    zval * object;
    zval function_name;

  protected:
    int getUserFunctionParamCount();
    int invokeUserFunction(zval *retval_ptr, int param_count, zval params[]);

  public:
    ClassMethodLambda(zval * object, const char * function_name_string) : object(object) {
      Z_ADDREF_P(object);
      ZVAL_STRING(&function_name, function_name_string);
    };
    ~ClassMethodLambda();
};

#ifdef __cplusplus
  } // extern "C"
#endif

#endif /* PHP_MUSTACHE_CLASS_METHOD_LAMBDA_HPP */

