
#ifndef PHP_MUSTACHE_LAMBDA_HPP
#define PHP_MUSTACHE_LAMBDA_HPP

#ifdef __cplusplus
   extern "C" {
#endif

class Lambda : public mustache::Lambda {
  protected:
    virtual int getUserFunctionParamCount() = 0;
#if PHP_MAJOR_VERSION >= 7
    virtual int invokeUserFunction(zval *retval_ptr, int param_count, zval params[]) = 0;
#else
    virtual int invokeUserFunction(zval **retval_ptr_ptr, int param_count, zval **params[] TSRMLS_DC) = 0;
#endif
  public:
    std::string invoke();
    std::string invoke(std::string * text, mustache::Renderer * renderer);
};

#ifdef __cplusplus
  } // extern "C"
#endif

#endif /* PHP_MUSTACHE_LAMBDA_HPP */

