
#ifndef PHP_MUSTACHE_LAMBDA_HELPER_HPP
#define PHP_MUSTACHE_LAMBDA_HELPER_HPP

#ifdef __cplusplus
   extern "C" {
#endif

#if PHP_MAJOR_VERSION < 7
struct php_obj_MustacheLambdaHelper {
    zend_object std;
    mustache::Renderer * renderer;
};
#else
struct php_obj_MustacheLambdaHelper {
    mustache::Renderer * renderer;
    zend_object std;
};
#endif

extern zend_class_entry * MustacheLambdaHelper_ce_ptr;

struct php_obj_MustacheLambdaHelper * php_mustache_lambda_helper_object_fetch_object(zval * zv TSRMLS_DC);

PHP_MINIT_FUNCTION(mustache_lambda_helper);

PHP_METHOD(MustacheLambdaHelper, __construct);
PHP_METHOD(MustacheLambdaHelper, render);

#ifdef __cplusplus
  } // extern "C"
#endif

#endif /* PHP_MUSTACHE_LAMBDA_HELPER_HPP */

