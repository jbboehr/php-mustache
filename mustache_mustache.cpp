
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_mustache.h"
#include "mustache_arginfo.h"
#if PHP_VERSION_ID < 80100
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#endif
#include "mustache_ast.hpp"
#include "mustache_data.hpp"
#include "mustache_exceptions.hpp"
#include "mustache_mustache.hpp"
#include "mustache_zval.hpp"
#include <cstddef>
#include <string>
#include <string_view>

/* {{{ ZE2 OO definitions */
zend_class_entry * Mustache_ce_ptr;
static zend_object_handlers Mustache_obj_handlers;
/* }}} */

#if PHP_VERSION_ID < 80100
PHP_METHOD(Mustache, __serialize)
{
  zend_throw_exception(zend_ce_exception,
      "Serialization of 'Mustache' is not allowed", 0);
  RETURN_THROWS();
}

PHP_METHOD(Mustache, __unserialize)
{
  zend_throw_exception(zend_ce_exception,
      "Serialization of 'Mustache' is not allowed", 0);
  RETURN_THROWS();
}
#endif

/* {{{ Mustache_methods */
static zend_function_entry Mustache_methods[] = {
#if PHP_VERSION_ID < 80100
  PHP_ME(Mustache, __serialize, arginfo_class_Mustache___serialize, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL)
  PHP_ME(Mustache, __unserialize, arginfo_class_Mustache___unserialize, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL)
#endif
  PHP_ME(Mustache, __construct, arginfo_class_Mustache___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(Mustache, getEscapeByDefault, arginfo_class_Mustache_getEscapeByDefault, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, getLambdaStringMode, arginfo_class_Mustache_getLambdaStringMode, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, getStartSequence, arginfo_class_Mustache_getStartSequence, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, getStopSequence, arginfo_class_Mustache_getStopSequence, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setEscapeByDefault, arginfo_class_Mustache_setEscapeByDefault, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setLambdaStringMode, arginfo_class_Mustache_setLambdaStringMode, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setStartSequence, arginfo_class_Mustache_setStartSequence, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setStopSequence, arginfo_class_Mustache_setStopSequence, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setPartialLimits, arginfo_class_Mustache_setPartialLimits, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, parse, arginfo_class_Mustache_parse, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, render, arginfo_class_Mustache_render, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, tokenize, arginfo_class_Mustache_tokenize, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, debugDataStructure, arginfo_class_Mustache_debugDataStructure, ZEND_ACC_PUBLIC)
#if defined(PHP_MUSTACHE_ARCHIVE_BENCHMARK)
  PHP_ME(Mustache, benchmarkSerializeArchive, arginfo_class_Mustache_benchmarkSerializeArchive, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, benchmarkRenderArchive, arginfo_class_Mustache_benchmarkRenderArchive, ZEND_ACC_PUBLIC)
#endif
  { NULL, NULL, NULL }
};
/* }}} */

/* {{{ php_mustache_mustache_object_fetch_object */
static inline struct php_obj_Mustache * php_mustache_mustache_fetch_object(zend_object * obj)
{
  return (struct php_obj_Mustache *) ((char *)(obj) - XtOffsetOf(struct php_obj_Mustache, std));
}

struct php_obj_Mustache * php_mustache_mustache_object_fetch_object(zval * zv)
{
  return php_mustache_mustache_fetch_object(Z_OBJ_P(zv));
}
/* }}} */

/* {{{ Mustache_obj_free */
static void Mustache_obj_free(zend_object * object)
{
  try {
    struct php_obj_Mustache * payload = php_mustache_mustache_fetch_object(object);

    if( payload->mustache != NULL ) {
      delete payload->mustache;
    }

    zend_object_std_dtor((zend_object *) object);
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} */

/* {{{ Mustache_obj_create */
static zend_object * Mustache_obj_create(zend_class_entry * ce)
{
  struct php_obj_Mustache * intern;

  try {
    intern = (struct php_obj_Mustache *) ecalloc(1, sizeof(php_obj_Mustache) + zend_object_properties_size(ce));
    intern->lambda_string_mode = mustache::LambdaStringMode::Template;
    intern->max_partial_entries = -1;
    intern->max_partial_text_bytes = -1;
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &Mustache_obj_handlers;

    intern->mustache = mustache_new_Mustache();
    return &intern->std;
  } catch(...) {
    mustache_exception_handler();
  }

  return NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mustache_mustache)
{
  try {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Mustache", Mustache_methods);
    ce.create_object = Mustache_obj_create;
#if PHP_VERSION_ID < 80100
    ce.serialize = zend_class_serialize_deny;
    ce.unserialize = zend_class_unserialize_deny;
#else
    ce.ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
#endif
    Mustache_ce_ptr = zend_register_internal_class(&ce);
    zend_declare_class_constant_long(Mustache_ce_ptr, ZEND_STRL("LAMBDA_STRING_TEMPLATE"),
        static_cast<zend_long>(mustache::LambdaStringMode::Template));
    zend_declare_class_constant_long(Mustache_ce_ptr, ZEND_STRL("LAMBDA_STRING_LITERAL"),
        static_cast<zend_long>(mustache::LambdaStringMode::Literal));
    memcpy(&Mustache_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    Mustache_obj_handlers.offset = XtOffsetOf(php_obj_Mustache, std);
    Mustache_obj_handlers.free_obj = Mustache_obj_free;
    Mustache_obj_handlers.clone_obj = NULL;

    return SUCCESS;
  } catch(...) {
    mustache_exception_handler();
    return FAILURE;
  }
}
/* }}} */

/* {{{ mustache_new_Mustache */
mustache::Mustache * mustache_new_Mustache() {
  mustache::Mustache * mustache = new mustache::Mustache();

  // Set ini settings
  if( MUSTACHEG(default_escape_by_default) ) {
    mustache->setEscapeByDefault(true);
  } else {
    mustache->setEscapeByDefault(false);
  }
  if( MUSTACHEG(default_start_sequence) ) {
    const char * start_sequence = MUSTACHEG(default_start_sequence);
    mustache->setStartSequence(std::string_view(start_sequence, strlen(start_sequence)));
  }
  if( MUSTACHEG(default_stop_sequence) ) {
    const char * stop_sequence = MUSTACHEG(default_stop_sequence);
    mustache->setStopSequence(std::string_view(stop_sequence, strlen(stop_sequence)));
  }

  return mustache;
}
/* }}} */

/* {{{ proto void Mustache::__construct() */
PHP_METHOD(Mustache, __construct)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::__construct */

/* {{{ proto int Mustache::getLambdaStringMode() */
PHP_METHOD(Mustache, getLambdaStringMode)
{
  try {
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, Mustache_ce_ptr) == FAILURE ) {
      throw PhpInvalidParameterException();
    }
    const php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    RETURN_LONG(static_cast<zend_long>(payload->lambda_string_mode));
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::getLambdaStringMode */

/* {{{ proto void Mustache::setLambdaStringMode(int mode) */
PHP_METHOD(Mustache, setLambdaStringMode)
{
  try {
    zend_long mode = 0;
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Ol",
            &_this_zval, Mustache_ce_ptr, &mode) == FAILURE ) {
      throw PhpInvalidParameterException();
    }
    if( mode != static_cast<zend_long>(mustache::LambdaStringMode::Template) &&
        mode != static_cast<zend_long>(mustache::LambdaStringMode::Literal) ) {
      mustache_argument_value_error(1, "must be Mustache::LAMBDA_STRING_TEMPLATE or Mustache::LAMBDA_STRING_LITERAL");
    }
    php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    // Active renders retain their snapshots, including the shared AST renderer.
    payload->lambda_string_mode = static_cast<mustache::LambdaStringMode>(mode);
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::setLambdaStringMode */

/* {{{ proto void Mustache::setPartialLimits(?int maxEntries = null, ?int maxTextBytes = null) */
PHP_METHOD(Mustache, setPartialLimits)
{
  try {
    zend_long max_entries = 0;
    zend_long max_text_bytes = 0;
    zend_bool entries_is_null = 1;
    zend_bool text_bytes_is_null = 1;
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O|l!l!",
            &_this_zval, Mustache_ce_ptr, &max_entries, &entries_is_null,
            &max_text_bytes, &text_bytes_is_null) == FAILURE ) {
      throw PhpInvalidParameterException();
    }
    if( !entries_is_null && max_entries < 0 ) {
      mustache_argument_value_error(1, "must be greater than or equal to 0, or null");
    }
    if( !text_bytes_is_null && max_text_bytes < 0 ) {
      mustache_argument_value_error(2, "must be greater than or equal to 0, or null");
    }
    php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    payload->max_partial_entries = entries_is_null ? -1 : max_entries;
    payload->max_partial_text_bytes = text_bytes_is_null ? -1 : max_text_bytes;
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::setPartialLimits */

/* {{{ proto boolean Mustache::getEscapeByDefault() */
PHP_METHOD(Mustache, getEscapeByDefault)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);

    // Main
    if( payload->mustache->getEscapeByDefault() ) {
      RETURN_TRUE;
    } else {
      RETURN_FALSE;
    }

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::getEscapeByDefault */

/* {{{ proto string Mustache::getStartSequence() */
PHP_METHOD(Mustache, getStartSequence)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);

    // Main
    const std::string & str = payload->mustache->getStartSequence();
    RETVAL_STRINGL(str.c_str(), str.length());

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::getStartSequence */

/* {{{ proto string Mustache::getStopSequence() */
PHP_METHOD(Mustache, getStopSequence)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);

    // Main
    const std::string & str = payload->mustache->getStopSequence();
    RETVAL_STRINGL(str.c_str(), str.length());

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::getStopSequence */

/* {{{ proto boolean Mustache::setEscapeByDefault(bool|int escapeByDefault) */
PHP_METHOD(Mustache, setEscapeByDefault)
{
  try {
    // Custom parameters
    zval * flag = NULL;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Oz",
            &_this_zval, Mustache_ce_ptr, &flag) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    zval * flagValue = mustache_dereference_zval(flag);
    bool escapeByDefault = false;
    if( flagValue != NULL
        && (Z_TYPE_P(flagValue) == IS_TRUE || Z_TYPE_P(flagValue) == IS_FALSE) ) {
      escapeByDefault = zend_is_true(flagValue);
    } else {
      zend_long integerFlag = 0;
      if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Ol",
              &_this_zval, Mustache_ce_ptr, &integerFlag) == FAILURE) {
        throw PhpInvalidParameterException();
      }
      escapeByDefault = integerFlag != 0;
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);

    // Main
    payload->mustache->setEscapeByDefault(escapeByDefault);
    RETURN_TRUE;

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::setEscapeByDefault */

/* {{{ proto boolean Mustache::setStartSequence(string str) */
PHP_METHOD(Mustache, setStartSequence)
{
  try {
    // Custom parameters
    char * str = NULL;
    size_t str_len = 0;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Os",
            &_this_zval, Mustache_ce_ptr, &str, &str_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);

    // Main
    payload->mustache->setStartSequence(std::string_view(str, str_len));
    RETURN_TRUE;

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::setStartSequence */

/* {{{ proto boolean Mustache::setStopSequence(string str) */
PHP_METHOD(Mustache, setStopSequence)
{
  try {
    // Custom parameters
    char * str = NULL;
    size_t str_len = 0;

    // Check parameters
    zval * _this_zval;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Os",
            &_this_zval, Mustache_ce_ptr, &str, &str_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);

    // Main
    payload->mustache->setStopSequence(std::string_view(str, str_len));
    RETURN_TRUE;

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::setStopSequence */

/* {{{ proto array Mustache::tokenize(string template) */
PHP_METHOD(Mustache, tokenize)
{
  try {
    // Custom parameters
    char * template_str = NULL;
    size_t template_len = 0;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Os",
            &_this_zval, Mustache_ce_ptr, &template_str, &template_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);

    // Tokenize template
    mustache::Node root;
    payload->mustache->tokenize(std::string_view(template_str, template_len), &root);

    // Convert to PHP array
    mustache_node_to_zval(root, return_value);

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::tokenize */

/* {{{ proto array Mustache::debugDataStructure(array data) */
PHP_METHOD(Mustache, debugDataStructure)
{
  try {
    // Custom parameters
    zval * data = NULL;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Oz",
            &_this_zval, Mustache_ce_ptr, &data) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();

    // Prepare template data
    mustache::Data templateData = mustache_data_from_zval(data);

    // Reverse template data
    mustache_data_to_zval(templateData, return_value);

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::debugDataStructure */
