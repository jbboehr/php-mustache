
#include "mustache_private.hpp"

#if PHP_MAJOR_VERSION >= 7
std::string Lambda::invoke()
{
  zval closure_result;
  std::string closure_result_string;

  if( invokeUserFunction(&closure_result, 0, NULL) == SUCCESS ) {
    convert_to_string(&closure_result);
    closure_result_string.assign(Z_STRVAL(closure_result));
    zval_dtor(&closure_result);
  }

  return closure_result_string;
}
#else
std::string Lambda::invoke()
{
  TSRMLS_FETCH();

  zval * closure_result_ptr = NULL;
  std::string closure_result_string;

  if( invokeUserFunction(&closure_result_ptr, 0, NULL TSRMLS_CC) == SUCCESS && closure_result_ptr != NULL ) {
    convert_to_string(closure_result_ptr);
    closure_result_string.assign(Z_STRVAL_P(closure_result_ptr));
    zval_ptr_dtor(&closure_result_ptr);
  }

  return closure_result_string;
}
#endif

#if PHP_MAJOR_VERSION >= 7
std::string Lambda::invoke(std::string * text, mustache::Renderer * renderer)
{
  TSRMLS_FETCH();

  zval closure_result;
  std::string closure_result_string;

  int param_count = 0;
  zval * params = NULL;

  param_count = std::min(getUserFunctionParamCount(), 2);
  if( param_count >= 0 ) {
    params = (zval *) safe_emalloc(sizeof(zval), param_count, 0);
  }
  if( param_count >= 1 ) {
    ZVAL_STRING(&params[0], text->c_str());
  }
  if( param_count >= 2 ) {
    object_init_ex(&params[1], MustacheLambdaHelper_ce_ptr);

    struct php_obj_MustacheLambdaHelper * payload = php_mustache_lambda_helper_object_fetch_object(&params[1] TSRMLS_CC);
    payload->renderer = renderer;
  }

  if( invokeUserFunction(&closure_result, param_count, params) == SUCCESS ) {
    convert_to_string(&closure_result);
    closure_result_string.assign(Z_STRVAL(closure_result));
  }
  zval_dtor(&closure_result);

  if( params != NULL ) {
    for( param_count = param_count - 1; param_count >= 0; param_count-- ) {
      zval_dtor(&params[param_count]);
    }

    efree(params);
  }

  return closure_result_string;
}
#else
std::string Lambda::invoke(std::string * text, mustache::Renderer * renderer)
{
  TSRMLS_FETCH();

  zval * closure_result_ptr = NULL;
  std::string closure_result_string;

  int param_count = 0;
  zval *** params = NULL;

  param_count = std::min(getUserFunctionParamCount(), 2);
  if( param_count >= 0 ) {
    params = (zval ***) safe_emalloc(sizeof(zval **), param_count, 0);
  }
  if( param_count >= 1 ) {
    params[0] = (zval **) emalloc(sizeof(zval **));

    MAKE_STD_ZVAL(*params[0]);
    ZVAL_STRING(*params[0], text->c_str(), 1);
  }
  if( param_count >= 2 ) {
    params[1] = (zval **) emalloc(sizeof(zval **));

    MAKE_STD_ZVAL(*params[1]);
    object_init_ex(*params[1], MustacheLambdaHelper_ce_ptr);

    struct php_obj_MustacheLambdaHelper * payload = php_mustache_lambda_helper_object_fetch_object(*params[1] TSRMLS_CC);
    payload->renderer = renderer;
  }

  if( invokeUserFunction(&closure_result_ptr, param_count, params TSRMLS_CC) == SUCCESS && closure_result_ptr != NULL ) {
    convert_to_string(closure_result_ptr);
    closure_result_string.assign(Z_STRVAL_P(closure_result_ptr));
    zval_ptr_dtor(&closure_result_ptr);
  }

  if( params != NULL ) {
    for( param_count = param_count - 1; param_count >= 0; param_count-- ) {
      zval_ptr_dtor(params[param_count]);
      efree(params[param_count]);
    }

    efree(params);
  }

  return closure_result_string;
}
#endif
