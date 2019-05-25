
#include "php_mustache.h"
#include "mustache_exceptions.hpp"
#include "mustache_lambda_helper.hpp"

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
static inline struct php_obj_MustacheLambdaHelper * php_mustache_lambda_helper_fetch_object(zend_object * obj)
{
  return (struct php_obj_MustacheLambdaHelper *)((char*)(obj) - XtOffsetOf(struct php_obj_MustacheLambdaHelper, std));
}

struct php_obj_MustacheLambdaHelper * php_mustache_lambda_helper_object_fetch_object(zval * zv)
{
  return php_mustache_lambda_helper_fetch_object(Z_OBJ_P(zv));
}
/* }}} */

/* {{{ MustacheLambdaHelper_obj_free */
static void MustacheLambdaHelper_obj_free(zend_object *object)
{
  try {
    zend_object_std_dtor((zend_object *)object);
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} */

/* {{{ MustacheLambdaHelper_obj_create */
static zend_object * MustacheLambdaHelper_obj_create(zend_class_entry * ce)
{
  struct php_obj_MustacheLambdaHelper * intern;

  try {
    intern = (struct php_obj_MustacheLambdaHelper *) ecalloc(1, sizeof(struct php_obj_MustacheLambdaHelper) + zend_object_properties_size(ce));
    zend_object_std_init(&intern->std, ce);
    intern->std.handlers = &MustacheLambdaHelper_obj_handlers;
    return &intern->std;
  } catch(...) {
    mustache_exception_handler();
  }
  return NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mustache_lambda_helper)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "MustacheLambdaHelper", MustacheLambdaHelper_methods);
  ce.create_object = MustacheLambdaHelper_obj_create;
  MustacheLambdaHelper_ce_ptr = zend_register_internal_class(&ce);
  memcpy(&MustacheLambdaHelper_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  MustacheLambdaHelper_obj_handlers.offset = XtOffsetOf(struct php_obj_MustacheLambdaHelper, std);
  MustacheLambdaHelper_obj_handlers.free_obj = MustacheLambdaHelper_obj_free;
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
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Os",
            &_this_zval, MustacheLambdaHelper_ce_ptr, &template_str, &template_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_MustacheLambdaHelper * payload = php_mustache_lambda_helper_object_fetch_object(_this_zval);

    std::string templateStr(template_str/*, (size_t) Z_STRLEN_P(template_str)*/);

    mustache::Node node;
    mustache::Tokenizer tokenizer;
    tokenizer.tokenize(&templateStr, &node);

    std::string output;
    output.reserve(template_len);

    payload->renderer->renderForLambda(&node, &output);

    RETURN_STRING(output.c_str());
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheLambdaHelper::render */

