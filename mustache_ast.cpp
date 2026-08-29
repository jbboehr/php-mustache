
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_mustache.h"
#include "mustache_arginfo.h"
#include "mustache_private.hpp"
#include "mustache_exceptions.hpp"
#include "mustache_ast.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* {{{ ZE2 OO definitions */
zend_class_entry * MustacheAST_ce_ptr;
static zend_object_handlers MustacheAST_obj_handlers;
/* }}} */

/* {{{ MustacheAST_methods */
static zend_function_entry MustacheAST_methods[] = {
  PHP_ME(MustacheAST, __construct, arginfo_class_MustacheAST___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheAST, fromBinary, arginfo_class_MustacheAST_fromBinary, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
  PHP_ME(MustacheAST, __sleep, arginfo_class_MustacheAST___sleep, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, toArray, arginfo_class_MustacheAST_toArray, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, toBinary, arginfo_class_MustacheAST_toBinary, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, __toString, arginfo_class_MustacheAST___toString, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, __wakeup, arginfo_class_MustacheAST___wakeup, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};
/* }}} */

namespace {

mustache::Node::SerializationLimits mustache_ast_serialization_limits()
{
  mustache::Node::SerializationLimits limits;
  limits.maxInputBytes = size_t{16} * 1024 * 1024;
  limits.maxOutputBytes = size_t{16} * 1024 * 1024;
  limits.maxNestingDepth = 64;
  limits.maxNodes = 100000;
  limits.maxDataPartsPerNode = 256;
  limits.maxDataParts = 100000;
  return limits;
}

} // namespace

/* {{{ mustache_node_from_binary_string */
std::unique_ptr<mustache::Node> mustache_node_from_binary_string(const char * str, size_t len)
{
  const std::string_view serial = len == 0 ? std::string_view() : std::string_view(str, len);
  return mustache::Node::unserializeOwned(serial, mustache_ast_serialization_limits());
}
/* }}} */

/* {{{ mustache_node_to_binary_string */
std::string mustache_node_to_binary_string(const mustache::Node& node)
{
  std::vector<uint8_t> serial = node.serializeValue(mustache_ast_serialization_limits());
  if( serial.empty() ) {
    return std::string();
  }
  return std::string(reinterpret_cast<const char *>(serial.data()), serial.size());
}
/* }}} */

/* {{{ mustache_node_to_zval */
void mustache_node_to_zval(const mustache::Node& node, zval * current)
{
  zval children = {0};
  zval child = {0};

  array_init(current);

  // Basic data
  add_assoc_long(current, "type", node.type);
  add_assoc_long(current, "flags", node.flags);
  if( node.data.has_value() && !node.data->empty() ) {
    add_assoc_stringl_ex(current, ZEND_STRL("data"),
        (char *) node.data->data(), node.data->length());
  }

  // Children
  if( !node.children.empty() ) {
    ZVAL_NULL(&children);
    array_init(&children);

    for( const std::unique_ptr<mustache::Node>& child_node : node.children ) {
      if( child_node == NULL ) {
        throw InvalidParameterException("MustacheAST contains an empty child node");
      }
      ZVAL_NULL(&child);
      mustache_node_to_zval(*child_node, &child);
      add_next_index_zval(&children, &child);
    }

    add_assoc_zval(current, "children", &children);
  }

  // Partials
  if( !node.partials.empty() ) {
    ZVAL_NULL(&children);
    array_init(&children);

    for( const auto& partial : node.partials ) {
      if( partial.second == NULL ) {
        throw InvalidParameterException("MustacheAST contains an empty partial node");
      }
      ZVAL_NULL(&child);
      mustache_node_to_zval(*partial.second, &child);
      add_assoc_zval_ex(&children, partial.first.c_str(), partial.first.length(), &child);
    }

    add_assoc_zval(current, "partials", &children);
  }
}
/* }}} */

/* {{{ php_mustache_ast_object_fetch_object */
static inline struct php_obj_MustacheAST * php_mustache_ast_fetch_object(zend_object * obj)
{
  return (struct php_obj_MustacheAST *)((char*)(obj) - XtOffsetOf(struct php_obj_MustacheAST, std));
}

struct php_obj_MustacheAST * php_mustache_ast_object_fetch_object(zval * zv)
{
  return php_mustache_ast_fetch_object(Z_OBJ_P(zv));
}
/* }}} */

/* {{{ MustacheAST_obj_free */
static void MustacheAST_obj_free(zend_object * object)
{
  try {
    struct php_obj_MustacheAST * payload = php_mustache_ast_fetch_object(object);

    delete payload->state;
    payload->state = NULL;

    zend_object_std_dtor((zend_object *)object);
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} */

/* {{{ MustacheAST_obj_create */
static zend_object * MustacheAST_obj_create(zend_class_entry * ce)
{
  struct php_obj_MustacheAST * intern;

  try {
    std::unique_ptr<php_mustache_ast_state> state =
        std::make_unique<php_mustache_ast_state>();
    intern = (struct php_obj_MustacheAST *) ecalloc(1, sizeof(struct php_obj_MustacheAST) + zend_object_properties_size(ce));
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &MustacheAST_obj_handlers;
    intern->state = state.release();
    return &intern->std;
  } catch(...) {
    mustache_exception_handler();
  }

  return NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mustache_ast)
{
  try {
    zend_class_entry ce;

    memcpy(&MustacheAST_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    MustacheAST_obj_handlers.offset = XtOffsetOf(struct php_obj_MustacheAST, std);
    MustacheAST_obj_handlers.free_obj = MustacheAST_obj_free;
    MustacheAST_obj_handlers.clone_obj = NULL;

    INIT_CLASS_ENTRY(ce, "MustacheAST", MustacheAST_methods);
    ce.create_object = MustacheAST_obj_create;

    MustacheAST_ce_ptr = zend_register_internal_class(&ce);
    MustacheAST_ce_ptr->create_object = MustacheAST_obj_create;

    zend_declare_property_null(MustacheAST_ce_ptr, ZEND_STRL("binaryString"), ZEND_ACC_PROTECTED);

    return SUCCESS;
  } catch(...) {
    mustache_exception_handler();
    return FAILURE;
  }
}
/* }}} */

/* {{{ proto void MustacheAST::__construct(string binaryString) */
PHP_METHOD(MustacheAST, __construct)
{
  try {
    // Custom parameters
    char * str = NULL;
    size_t str_len = 0;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O|s!",
            &_this_zval, MustacheAST_ce_ptr, &str, &str_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval);

    // Check payload
    if( payload->state == NULL ) {
      throw InvalidParameterException("MustacheAST state was not initialized properly");
    }
    if( payload->state->node != NULL ) {
      throw InvalidParameterException("MustacheAST is already initialized");
    }

    // Unserialize
    payload->state->node = mustache_node_from_binary_string(str, str_len);

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::__construct */

/* {{{ proto static MustacheAST MustacheAST::fromBinary(string binary) */
PHP_METHOD(MustacheAST, fromBinary)
{
  try {
    char * str = NULL;
    size_t str_len = 0;

    if( zend_parse_parameters(ZEND_NUM_ARGS(), (char *) "s", &str, &str_len) == FAILURE ) {
      throw PhpInvalidParameterException();
    }

    std::unique_ptr<mustache::Node> node =
        mustache_node_from_binary_string(str, str_len);

    zend_class_entry * called_scope = zend_get_called_scope(execute_data);
    if( called_scope == NULL ||
        !instanceof_function(called_scope, MustacheAST_ce_ptr) ) {
      throw InvalidParameterException("MustacheAST factory called outside its class scope");
    }

    if( object_init_ex(return_value, called_scope) != SUCCESS ) {
      return;
    }
    struct php_obj_MustacheAST * payload =
        php_mustache_ast_object_fetch_object(return_value);
    if( payload->state == NULL ) {
      throw InvalidParameterException("MustacheAST state was not initialized properly");
    }
    if( payload->state->node != NULL ) {
      throw InvalidParameterException("MustacheAST is already initialized");
    }

    payload->state->node = std::move(node);
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::fromBinary */

/* {{{ proto void MustacheAST::__sleep() */
PHP_METHOD(MustacheAST, __sleep)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, MustacheAST_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval);

    array_init(return_value);

    // Check payload
    if( payload->state != NULL && payload->state->node != NULL ) {
      // Serialize and store
      std::string serial = mustache_node_to_binary_string(*payload->state->node);
#if PHP_VERSION_ID < 80000
      zend_update_property_stringl(MustacheAST_ce_ptr, _this_zval, ZEND_STRL("binaryString"),
          serial.data(), serial.size());
#else
      zend_update_property_stringl(MustacheAST_ce_ptr, Z_OBJ_P(_this_zval), ZEND_STRL("binaryString"),
          serial.data(), serial.size());
#endif
      add_next_index_string(return_value, "binaryString");
    }

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::__sleep */

/* {{{ proto array MustacheAST::toArray() */
PHP_METHOD(MustacheAST, toArray)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, MustacheAST_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval);

    // Check payload
    if( payload->state == NULL || payload->state->node == NULL ) {
      throw InvalidParameterException("MustacheAST was not initialized properly");
    }

    // Convert to PHP array
    mustache_node_to_zval(*payload->state->node, return_value);

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::toArray */

/* {{{ proto string MustacheAST::toBinary() */
PHP_METHOD(MustacheAST, toBinary)
{
  try {
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, MustacheAST_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    _this_zval = getThis();
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval);

    if( payload->state == NULL || payload->state->node == NULL ) {
      throw InvalidParameterException("MustacheAST was not initialized properly");
    }

    std::string serial = mustache_node_to_binary_string(*payload->state->node);
    RETVAL_STRINGL(serial.data(), serial.size());
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::toBinary */

/* {{{ proto string MustacheAST::__toString() */
PHP_METHOD(MustacheAST, __toString)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, MustacheAST_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval);

    // Check payload
    if( payload->state == NULL || payload->state->node == NULL ) {
      throw InvalidParameterException("MustacheAST was not initialized properly");
    }

    // Convert to PHP binary string
    std::string serial = mustache_node_to_binary_string(*payload->state->node);
    RETVAL_STRINGL(serial.data(), serial.size());

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::__toString */

/* {{{ proto void MustacheAST::__wakeup() */
static inline void php_mustache_ast_wakeup(zval * _this_zval)
{
    zval rv;
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval);
#if PHP_VERSION_ID < 80000
    zval * value = zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, "binaryString", sizeof("binaryString")-1, 1, &rv);
#else
    zval * value = zend_read_property(Z_OBJCE_P(_this_zval), Z_OBJ_P(_this_zval), "binaryString", sizeof("binaryString")-1, 1, &rv);
#endif

    if( payload->state == NULL ) {
      throw InvalidParameterException("MustacheAST state was not initialized properly");
    }
    if( Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0 ) {
      if( payload->state->node != NULL ) {
        throw InvalidParameterException("MustacheAST is already initialized");
      }
      payload->state->node =
          mustache_node_from_binary_string(Z_STRVAL_P(value), Z_STRLEN_P(value));
    }
}

PHP_METHOD(MustacheAST, __wakeup)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, MustacheAST_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    php_mustache_ast_wakeup(getThis());
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::__wakeup */
