#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_mustache.h"
#include "mustache_arginfo.h"
#include "mustache_exceptions.hpp"
#include "mustache_lambda_result.hpp"
#if PHP_VERSION_ID < 80100
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#endif

zend_class_entry * MustacheLiteralResult_ce_ptr;
zend_class_entry * MustacheTemplateResult_ce_ptr;
static zend_object_handlers MustacheLambdaResult_obj_handlers;

struct php_obj_MustacheLambdaResult {
  zend_string * text;
  zend_object std;
};

static php_obj_MustacheLambdaResult * mustache_lambda_result_fetch(zend_object * object)
{
  return (php_obj_MustacheLambdaResult *)((char *) object - XtOffsetOf(php_obj_MustacheLambdaResult, std));
}

zend_string * php_mustache_lambda_result_text(zval * value)
{
  zend_string * text = mustache_lambda_result_fetch(Z_OBJ_P(value))->text;
  if( text == NULL ) {
    zend_throw_error(NULL, "%s is not initialized", ZSTR_VAL(Z_OBJCE_P(value)->name));
    throw PhpInvalidParameterException();
  }
  return text;
}

static zend_object * MustacheLambdaResult_obj_create(zend_class_entry * ce)
{
  php_obj_MustacheLambdaResult * payload =
      (php_obj_MustacheLambdaResult *) zend_object_alloc(sizeof(php_obj_MustacheLambdaResult), ce);
  payload->text = NULL;
  zend_object_std_init(&payload->std, ce);
  object_properties_init(&payload->std, ce);
  payload->std.handlers = &MustacheLambdaResult_obj_handlers;
  return &payload->std;
}

static void MustacheLambdaResult_obj_free(zend_object * object)
{
  php_obj_MustacheLambdaResult * payload = mustache_lambda_result_fetch(object);
  if( payload->text != NULL ) {
    zend_string_release(payload->text);
  }
  zend_object_std_dtor(object);
}

static zend_object * MustacheLambdaResult_obj_clone(zend_object * object)
{
  zend_string * text = mustache_lambda_result_fetch(object)->text;
  if( text == NULL ) {
    zend_throw_error(NULL, "%s is not initialized", ZSTR_VAL(object->ce->name));
    return NULL;
  }
  zend_object * copy = MustacheLambdaResult_obj_create(object->ce);
  mustache_lambda_result_fetch(copy)->text = zend_string_copy(text);
  zend_objects_clone_members(copy, object);
  return copy;
}

static int MustacheLambdaResult_obj_compare(zval * left, zval * right)
{
  ZEND_COMPARE_OBJECTS_FALLBACK(left, right);
  if( Z_OBJCE_P(left) != Z_OBJCE_P(right) ) {
    return ZEND_UNCOMPARABLE;
  }
  if( Z_OBJ_P(left) == Z_OBJ_P(right) ) {
    return 0;
  }
  zend_string * left_text = mustache_lambda_result_fetch(Z_OBJ_P(left))->text;
  zend_string * right_text = mustache_lambda_result_fetch(Z_OBJ_P(right))->text;
  if( left_text == NULL || right_text == NULL ) {
    zend_throw_error(NULL, "%s is not initialized", ZSTR_VAL(Z_OBJCE_P(left)->name));
    return ZEND_UNCOMPARABLE;
  }
  return zend_binary_strcmp(ZSTR_VAL(left_text), ZSTR_LEN(left_text),
      ZSTR_VAL(right_text), ZSTR_LEN(right_text));
}

PHP_METHOD(MustacheLiteralResult, __construct)
{
  zend_string * text;
  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(text)
  ZEND_PARSE_PARAMETERS_END();

  php_obj_MustacheLambdaResult * payload = mustache_lambda_result_fetch(Z_OBJ_P(getThis()));
  if( payload->text != NULL ) {
    zend_throw_error(NULL, "%s is already initialized", ZSTR_VAL(Z_OBJCE_P(getThis())->name));
    RETURN_THROWS();
  }
  payload->text = zend_string_copy(text);
}

PHP_METHOD(MustacheLiteralResult, getText)
{
  ZEND_PARSE_PARAMETERS_NONE();
  try {
    RETURN_STR_COPY(php_mustache_lambda_result_text(getThis()));
  } catch(...) {
    mustache_exception_handler();
  }
}

#if PHP_VERSION_ID < 80100
PHP_METHOD(MustacheLiteralResult, __serialize)
{
  zend_throw_exception_ex(zend_ce_exception, 0,
      "Serialization of '%s' is not allowed", ZSTR_VAL(Z_OBJCE_P(getThis())->name));
  RETURN_THROWS();
}

PHP_METHOD(MustacheLiteralResult, __unserialize)
{
  zend_throw_exception_ex(zend_ce_exception, 0,
      "Serialization of '%s' is not allowed", ZSTR_VAL(Z_OBJCE_P(getThis())->name));
  RETURN_THROWS();
}
#endif

static zend_function_entry MustacheLiteralResult_methods[] = {
  PHP_ME(MustacheLiteralResult, __construct, arginfo_class_MustacheLiteralResult___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheLiteralResult, getText, arginfo_class_MustacheLiteralResult_getText, ZEND_ACC_PUBLIC)
#if PHP_VERSION_ID < 80100
  PHP_ME(MustacheLiteralResult, __serialize, arginfo_class_MustacheLiteralResult___serialize, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL)
  PHP_ME(MustacheLiteralResult, __unserialize, arginfo_class_MustacheLiteralResult___unserialize, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL)
#endif
  { NULL, NULL, NULL }
};

// The classes have identical methods and storage; their class selects interpretation.
static zend_function_entry MustacheTemplateResult_methods[] = {
  PHP_MALIAS(MustacheLiteralResult, __construct, __construct, arginfo_class_MustacheTemplateResult___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_MALIAS(MustacheLiteralResult, getText, getText, arginfo_class_MustacheTemplateResult_getText, ZEND_ACC_PUBLIC)
#if PHP_VERSION_ID < 80100
  PHP_MALIAS(MustacheLiteralResult, __serialize, __serialize, arginfo_class_MustacheTemplateResult___serialize, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL)
  PHP_MALIAS(MustacheLiteralResult, __unserialize, __unserialize, arginfo_class_MustacheTemplateResult___unserialize, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL)
#endif
  { NULL, NULL, NULL }
};

PHP_MINIT_FUNCTION(mustache_lambda_result)
{
  memcpy(&MustacheLambdaResult_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  MustacheLambdaResult_obj_handlers.offset = XtOffsetOf(php_obj_MustacheLambdaResult, std);
  MustacheLambdaResult_obj_handlers.free_obj = MustacheLambdaResult_obj_free;
  MustacheLambdaResult_obj_handlers.clone_obj = MustacheLambdaResult_obj_clone;
  MustacheLambdaResult_obj_handlers.compare = MustacheLambdaResult_obj_compare;

  zend_class_entry ce;
  INIT_CLASS_ENTRY(ce, "MustacheLiteralResult", MustacheLiteralResult_methods);
  ce.ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
  ce.create_object = MustacheLambdaResult_obj_create;
#if PHP_VERSION_ID < 80100
  ce.serialize = zend_class_serialize_deny;
  ce.unserialize = zend_class_unserialize_deny;
#else
  ce.ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
#endif
  MustacheLiteralResult_ce_ptr = zend_register_internal_class(&ce);

  INIT_CLASS_ENTRY(ce, "MustacheTemplateResult", MustacheTemplateResult_methods);
  ce.ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
  ce.create_object = MustacheLambdaResult_obj_create;
#if PHP_VERSION_ID < 80100
  ce.serialize = zend_class_serialize_deny;
  ce.unserialize = zend_class_unserialize_deny;
#else
  ce.ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
#endif
  MustacheTemplateResult_ce_ptr = zend_register_internal_class(&ce);
  return SUCCESS;
}
