
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_mustache.h"
#include "mustache_private.hpp"
#include "mustache_exceptions.hpp"
#include "mustache_ast.hpp"

/* {{{ ZE2 OO definitions */
zend_class_entry * MustacheAST_ce_ptr;
static zend_object_handlers MustacheAST_obj_handlers;
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(MustacheAST____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheAST____sleep_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheAST__toArray_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

#if PHP_VERSION_ID >= 80200
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(MustacheAST____toString_args, 0, 0, IS_STRING, 0)
#else
ZEND_BEGIN_ARG_INFO_EX(MustacheAST____toString_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
#endif
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheAST____wakeup_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ MustacheAST_methods */
static zend_function_entry MustacheAST_methods[] = {
  PHP_ME(MustacheAST, __construct, MustacheAST____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheAST, __sleep, MustacheAST____sleep_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, toArray, MustacheAST__toArray_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, __toString, MustacheAST____toString_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, __wakeup, MustacheAST____wakeup_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};
/* }}} */

/* {{{ mustache_node_from_binary_string */
void mustache_node_from_binary_string(mustache::Node ** node, char * str, int len)
{
  std::vector<uint8_t> uint_str;
  uint_str.resize(len);
  int i = 0;
  for( i = 0; i < len; i++ ) {
    uint_str[i] = str[i];
  }

  size_t vpos = 0;
  *node = mustache::Node::unserialize(uint_str, 0, &vpos);
}
/* }}} */

/* {{{ mustache_node_to_binary_string */
void mustache_node_to_binary_string(mustache::Node * node, char ** estr, int * elen)
{
  std::vector<uint8_t> * serialPtr = node->serialize();
  std::vector<uint8_t> & serial = *serialPtr;
  int serialLen = serial.size();

  char * str = (char *) emalloc(sizeof(char *) * (serialLen + 1));
  for( int i = 0 ; i < serialLen; i++ ) {
    str[i] = (char) serial[i];
  }
  str[serialLen] = '\0';
  delete serialPtr;

  *elen = serialLen;
  *estr = str;
}
/* }}} */

/* {{{ mustache_node_to_zval */
void mustache_node_to_zval(mustache::Node * node, zval * current)
{
  zval children = {0};
  zval child = {0};

  array_init(current);

  // Basic data
  add_assoc_long(current, "type", node->type);
  add_assoc_long(current, "flags", node->flags);
  if( NULL != node->data && node->data->length() > 0 ) {
    add_assoc_stringl_ex(current, ZEND_STRL("data"), (char *) node->data->c_str(), node->data->length());
  }

  // Children
  if( node->children.size() > 0 ) {
    ZVAL_NULL(&children);
    array_init(&children);

    mustache::Node::Children::iterator it;
    for ( it = node->children.begin() ; it != node->children.end(); it++ ) {
      ZVAL_NULL(&child);
      mustache_node_to_zval(*it, &child);
      add_next_index_zval(&children, &child);
    }

    add_assoc_zval(current, "children", &children);
  }

  // Partials
  if( node->partials.size() > 0 ) {
    ZVAL_NULL(&children);
    array_init(&children);

    mustache::Node::Partials::iterator it;
    for ( it = node->partials.begin() ; it != node->partials.end(); it++ ) {
      ZVAL_NULL(&child);
      mustache_node_to_zval(&(it->second), &child);
      add_assoc_zval(&children, it->first.c_str(), &child);
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

    if( payload->node != NULL ) {
      delete payload->node;
    }

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
    intern = (struct php_obj_MustacheAST *) ecalloc(1, sizeof(struct php_obj_MustacheAST) + zend_object_properties_size(ce));
    zend_object_std_init(&intern->std, ce);
    intern->std.handlers = &MustacheAST_obj_handlers;
    intern->node = NULL;
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
    long str_len = 0;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O|s",
            &_this_zval, MustacheAST_ce_ptr, &str, &str_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval);

    // Check payload
    if( payload->node != NULL ) {
      throw InvalidParameterException("MustacheAST is already initialized");
    }

    // Unserialize
    mustache_node_from_binary_string(&payload->node, str, str_len);

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::__construct */

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
    if( payload->node != NULL ) {
      // Serialize and store
      char * str = NULL;
      int len = 0;
      mustache_node_to_binary_string(payload->node, &str, &len);
      if( str != NULL ) {
#if PHP_VERSION_ID < 80000
        zend_update_property_stringl(MustacheAST_ce_ptr, _this_zval, ZEND_STRL("binaryString"), str, len);
#else
        zend_update_property_stringl(MustacheAST_ce_ptr, Z_OBJ_P(_this_zval), ZEND_STRL("binaryString"), str, len);
#endif
        add_next_index_string(return_value, "binaryString");
        efree(str);
      }
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
    if( payload->node == NULL ) {
      throw InvalidParameterException("MustacheAST was not initialized properly");
    }

    // Convert to PHP array
    mustache_node_to_zval(payload->node, return_value);

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::toArray */

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
    if( payload->node == NULL ) {
      throw InvalidParameterException("MustacheAST was not initialized properly");
    }

    // Convert to PHP binary string
    char * str = NULL;
    int len = 0;
    mustache_node_to_binary_string(payload->node, &str, &len);

    if( str != NULL ) {
      RETVAL_STRINGL(str, len);
      efree(str);
    }

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::__toString */

/* {{{ proto void MustacheAST::__wakeup() */
static inline void php_mustache_ast_wakeup(zval * _this_zval, zval * return_value)
{
    zval rv;
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval);
#if PHP_VERSION_ID < 80000
    zval * value = zend_read_property(Z_OBJCE_P(_this_zval), _this_zval, "binaryString", sizeof("binaryString")-1, 1, &rv);
#else
    zval * value = zend_read_property(Z_OBJCE_P(_this_zval), Z_OBJ_P(_this_zval), "binaryString", sizeof("binaryString")-1, 1, &rv);
#endif

    if( Z_TYPE_P(value) == IS_STRING && Z_STRLEN_P(value) > 0 ) {
    	mustache_node_from_binary_string(&payload->node, Z_STRVAL_P(value), Z_STRLEN_P(value));
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

    php_mustache_ast_wakeup(getThis(), return_value);
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheAST::__wakeup */

