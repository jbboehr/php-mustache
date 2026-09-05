
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_mustache.h"
#include "mustache_arginfo.h"
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
  PHP_ME(MustacheAST, __serialize, arginfo_class_MustacheAST___serialize, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, __unserialize, arginfo_class_MustacheAST___unserialize, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, toArray, arginfo_class_MustacheAST_toArray, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, toBinary, arginfo_class_MustacheAST_toBinary, ZEND_ACC_PUBLIC)
  PHP_MALIAS(MustacheAST, __toString, toBinary, arginfo_class_MustacheAST___toString, ZEND_ACC_PUBLIC)
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

[[noreturn]] void mustache_ast_value_error(const char * message)
{
  zend_value_error("%s", message);
  throw PhpInvalidParameterException();
}

} // namespace

/* {{{ mustache_node_from_binary_string */
static std::unique_ptr<mustache::Node> mustache_node_from_binary_string(const char * str, size_t len)
{
  const std::string_view serial = len == 0 ? std::string_view() : std::string_view(str, len);
  return mustache::Node::unserializeOwned(serial, mustache_ast_serialization_limits());
}
/* }}} */

/* {{{ mustache_node_to_zend_string */
static zend_string * mustache_node_to_zend_string(const mustache::Node& node)
{
  std::vector<uint8_t> serial = node.serializeValue(mustache_ast_serialization_limits());
  const char * data = serial.empty()
      ? ""
      : reinterpret_cast<const char *>(serial.data());
  return zend_string_init(data, serial.size(), 0);
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
    intern = (struct php_obj_MustacheAST *) zend_object_alloc(sizeof(struct php_obj_MustacheAST), ce);
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

/* {{{ proto array MustacheAST::__serialize() */
PHP_METHOD(MustacheAST, __serialize)
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
      mustache_ast_value_error("MustacheAST is not initialized");
    }

    array_init(return_value);
    add_assoc_str(return_value, "binary", mustache_node_to_zend_string(*payload->state->node));
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::__serialize */

/* {{{ proto void MustacheAST::__unserialize(array data) */
PHP_METHOD(MustacheAST, __unserialize)
{
  try {
    zval * _this_zval = NULL;
    zval * data = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Oa",
            &_this_zval, MustacheAST_ce_ptr, &data) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    _this_zval = getThis();
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval);
    if( payload->state == NULL ) {
      mustache_ast_value_error("MustacheAST state is not initialized");
    }
    if( payload->state->node != NULL ) {
      mustache_ast_value_error("MustacheAST is already initialized");
    }

    HashTable * values = Z_ARRVAL_P(data);
    if( zend_hash_num_elements(values) != 1 ) {
      mustache_ast_value_error(
          "MustacheAST serialization data must contain exactly one binary value");
    }

    zval * value = zend_hash_str_find(values, ZEND_STRL("binary"));
    if( value == NULL ) {
      static const char legacyKey[] = "\0*\0binaryString";
      value = zend_hash_str_find(values, legacyKey, sizeof(legacyKey) - 1);
    }
    if( value == NULL ) {
      value = zend_hash_str_find(values, ZEND_STRL("binaryString"));
    }
    if( value == NULL ) {
      mustache_ast_value_error(
          "MustacheAST serialization data must contain exactly one binary string");
    }
    ZVAL_DEREF(value);
    if( Z_TYPE_P(value) != IS_STRING ) {
      mustache_ast_value_error(
          "MustacheAST serialization data must contain exactly one binary string");
    }

    std::unique_ptr<mustache::Node> node = mustache_node_from_binary_string(
        Z_STRVAL_P(value), Z_STRLEN_P(value));
    payload->state->node = std::move(node);
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::__unserialize */

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
      mustache_ast_value_error("MustacheAST is not initialized");
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
      mustache_ast_value_error("MustacheAST is not initialized");
    }

    RETVAL_STR(mustache_node_to_zend_string(*payload->state->node));
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::toBinary */
