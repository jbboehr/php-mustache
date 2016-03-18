
#include "mustache_private.hpp"

/* {{{ ZE2 OO definitions */
zend_class_entry * MustacheLambdaHelper_ce_ptr;
static zend_object_handlers MustacheLambdaHelper_obj_handlers;
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(MustacheLambdaHelper____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheLambdaHelper__render_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ MustacheLambdaHelper_methods */
static zend_function_entry MustacheLambdaHelper_methods[] = {
  PHP_ME(MustacheLambdaHelper, __construct, MustacheLambdaHelper____construct_args, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
  PHP_ME(MustacheLambdaHelper, render, MustacheLambdaHelper__render_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};
/* }}} */

/* {{{ php_mustache_lambda_helper_object_fetch_object */
#if PHP_MAJOR_VERSION < 7
struct php_obj_MustacheLambdaHelper * php_mustache_lambda_helper_object_fetch_object(zval * zv TSRMLS_DC)
{
  return (struct php_obj_MustacheLambdaHelper *) zend_object_store_get_object(zv TSRMLS_CC);
}
#else
static inline struct php_obj_MustacheLambdaHelper * php_mustache_lambda_helper_fetch_object(zend_object * obj TSRMLS_DC)
{
  return (struct php_obj_MustacheLambdaHelper *)((char*)(obj) - XtOffsetOf(struct php_obj_MustacheLambdaHelper, std));
}

struct php_obj_MustacheLambdaHelper * php_mustache_lambda_helper_object_fetch_object(zval * zv TSRMLS_DC)
{
  return php_mustache_lambda_helper_fetch_object(Z_OBJ_P(zv) TSRMLS_CC);
}
#endif
/* }}} */

/* {{{ MustacheLambdaHelper_obj_free */
#if PHP_MAJOR_VERSION < 7
static void MustacheLambdaHelper_obj_free(void *object TSRMLS_DC)
{
  try {
    efree(object);
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
#else
static void MustacheLambdaHelper_obj_free(zend_object *object TSRMLS_DC)
{
  try {
    zend_object_std_dtor((zend_object *)object TSRMLS_CC);
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
#endif
/* }}} */

/* {{{ MustacheLambdaHelper_obj_create */
#if PHP_MAJOR_VERSION < 7
static zend_object_value MustacheLambdaHelper_obj_create(zend_class_entry *class_type TSRMLS_DC)
{
  zend_object_value retval;

  try {
    struct php_obj_MustacheLambdaHelper * payload;

    payload = (struct php_obj_MustacheLambdaHelper *) emalloc(sizeof(struct php_obj_MustacheLambdaHelper));
    memset(payload, 0, sizeof(struct php_obj_MustacheLambdaHelper));
    payload->std.ce = class_type;

    payload->renderer = NULL;

    retval.handle = zend_objects_store_put(payload, NULL, (zend_objects_free_object_storage_t) MustacheLambdaHelper_obj_free, NULL TSRMLS_CC);
    retval.handlers = &MustacheLambdaHelper_obj_handlers;

  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }

  return retval;
}
#else
static zend_object * MustacheLambdaHelper_obj_create(zend_class_entry * ce TSRMLS_DC)
{
  struct php_obj_MustacheLambdaHelper * intern;

  try {
    intern = (struct php_obj_MustacheLambdaHelper *) ecalloc(1, sizeof(struct php_obj_MustacheLambdaHelper) + zend_object_properties_size(ce));
    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    intern->std.handlers = &MustacheLambdaHelper_obj_handlers;
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }

  return &intern->std;
}
#endif
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mustache_lambda_helper)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "MustacheLambdaHelper", MustacheLambdaHelper_methods);
  ce.create_object = MustacheLambdaHelper_obj_create;
  MustacheLambdaHelper_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&MustacheLambdaHelper_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
#if PHP_MAJOR_VERSION >= 7
    MustacheLambdaHelper_obj_handlers.offset = XtOffsetOf(struct php_obj_MustacheLambdaHelper, std);
    MustacheLambdaHelper_obj_handlers.free_obj = MustacheLambdaHelper_obj_free;
#endif
  MustacheLambdaHelper_obj_handlers.clone_obj = NULL;

  return SUCCESS;
}
/* }}} */

/* {{{ proto void MustacheLambdaHelper::__construct() */
PHP_METHOD(MustacheLambdaHelper, __construct)
{
  // placeholder to prevent userland instantiation
}
/* }}} MustacheLambdaHelper::__construct */

/* {{{ proto void MustacheLambdaHelper::render(string tmpl) */
PHP_METHOD(MustacheLambdaHelper, render)
{
  try {
    // Custom parameters
    char * template_str = NULL;
    long template_len = 0;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "Os",
            &_this_zval, MustacheLambdaHelper_ce_ptr, &template_str, &template_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    struct php_obj_MustacheLambdaHelper * payload = php_mustache_lambda_helper_object_fetch_object(_this_zval TSRMLS_CC);

    std::string templateStr(template_str/*, (size_t) Z_STRLEN_P(template_str)*/);

    mustache::Node node;
    mustache::Tokenizer tokenizer;
    tokenizer.tokenize(&templateStr, &node);

    std::string output;
    output.reserve(template_len);

    payload->renderer->renderForLambda(&node, &output);

#if PHP_MAJOR_VERSION >= 7
    RETURN_STRING(output.c_str());
#else
    RETURN_STRING(output.c_str(), 1);
#endif

  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} MustacheLambdaHelper::render */

