
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_mustache.h"
#include "mustache_private.hpp"
#include "mustache_ast.hpp"
#include "mustache_data.hpp"
#include "mustache_exceptions.hpp"
#include "mustache_template.hpp"
#include "mustache_mustache.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#if defined(PHP_MUSTACHE_ARCHIVE_BENCHMARK)
#include <mustache/archived_template.hpp>
#include <cstdint>
#include <vector>
#endif

/* {{{ ZE2 OO definitions */
zend_class_entry * Mustache_ce_ptr;
static zend_object_handlers Mustache_obj_handlers;
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(Mustache____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__getEscapeByDefault_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__getStartSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__getStopSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__setEscapeByDefault_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, escapeByDefault)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__setStartSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, startSequence)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__setStopSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, stopSequence)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__parse_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__render_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 3)
	ZEND_ARG_INFO(0, str)
        ZEND_ARG_INFO(0, vars)
        ZEND_ARG_INFO(0, partials)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__tokenize_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__debugDataStructure_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
        ZEND_ARG_INFO(0, vars)
ZEND_END_ARG_INFO()

#if defined(PHP_MUSTACHE_ARCHIVE_BENCHMARK)
ZEND_BEGIN_ARG_INFO_EX(Mustache__benchmarkSerializeArchive_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
        ZEND_ARG_INFO(0, tmpl)
        ZEND_ARG_ARRAY_INFO(0, partials, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__benchmarkRenderArchive_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
        ZEND_ARG_INFO(0, archive)
        ZEND_ARG_INFO(0, vars)
ZEND_END_ARG_INFO()
#endif
/* }}} */

/* {{{ Mustache_methods */
static zend_function_entry Mustache_methods[] = {
  PHP_ME(Mustache, __construct, Mustache____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(Mustache, getEscapeByDefault, Mustache__getEscapeByDefault_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, getStartSequence, Mustache__getStartSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, getStopSequence, Mustache__getStopSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setEscapeByDefault, Mustache__setEscapeByDefault_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setStartSequence, Mustache__setStartSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setStopSequence, Mustache__setStopSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, parse, Mustache__parse_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, render, Mustache__render_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, tokenize, Mustache__tokenize_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, debugDataStructure, Mustache__debugDataStructure_args, ZEND_ACC_PUBLIC)
#if defined(PHP_MUSTACHE_ARCHIVE_BENCHMARK)
  PHP_ME(Mustache, benchmarkSerializeArchive, Mustache__benchmarkSerializeArchive_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, benchmarkRenderArchive, Mustache__benchmarkRenderArchive_args, ZEND_ACC_PUBLIC)
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
    zend_object_std_init(&intern->std, ce);
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
    Mustache_ce_ptr = zend_register_internal_class(&ce);
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

/* {{{ mustache_parse_data_param */
bool mustache_parse_data_param(zval * data, mustache::Mustache * mustache, mustache::Data ** node)
{
  struct php_obj_MustacheData * mdPayload = NULL;

  if( Z_TYPE_P(data) == IS_OBJECT ) {
    if( Z_OBJCE_P(data) == MustacheData_ce_ptr ) {
      mdPayload = php_mustache_data_object_fetch_object(data);
      if( mdPayload->data == NULL ) {
        throw InvalidParameterException("MustacheData was not initialized properly");
      }
      *node = mdPayload->data;
      return true;
    } else {
      **node = mustache_data_from_zval(data);
      return true;
    }
  } else {
    **node = mustache_data_from_zval(data);
    return true;
  }
}
/* }}} */

namespace {

struct NodeCloneState {
    size_t nodes = 0;
    size_t dataParts = 0;
};

static zval * mustache_dereference_zval(zval * value)
{
  if( value == NULL ) {
    return NULL;
  }
  if( Z_TYPE_P(value) == IS_INDIRECT ) {
    value = Z_INDIRECT_P(value);
  }
  ZVAL_DEREF(value);
  return value;
}

static bool mustache_is_ast(zval * value)
{
  value = mustache_dereference_zval(value);
  return value != NULL && Z_TYPE_P(value) == IS_OBJECT &&
      Z_OBJCE_P(value) == MustacheAST_ce_ptr;
}

static const mustache::Node * mustache_ast_node(zval * value)
{
  value = mustache_dereference_zval(value);
  struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(value);
  if( payload->state == NULL || payload->state->node == NULL ) {
    php_error(E_WARNING, "Empty MustacheAST");
    return NULL;
  }
  return payload->state->node.get();
}

static std::string mustache_template_object_source(zval * value)
{
  zval rv;
#if PHP_VERSION_ID < 80000
  zval * source_value = zend_read_property(
      Z_OBJCE_P(value), value, "template", sizeof("template") - 1, 1, &rv);
#else
  zval * source_value = zend_read_property(
      Z_OBJCE_P(value), Z_OBJ_P(value), "template", sizeof("template") - 1, 1, &rv);
#endif
  zend_string * source_string = zval_get_string(source_value);
  std::string source(ZSTR_VAL(source_string), ZSTR_LEN(source_string));
  zend_string_release(source_string);
  return source;
}

static bool mustache_template_source(zval * value, std::string& source)
{
  value = mustache_dereference_zval(value);
  if( value == NULL ) {
    php_error(E_WARNING, "Invalid argument");
    return false;
  }
  if( Z_TYPE_P(value) == IS_STRING ) {
    source.assign(Z_STRVAL_P(value), Z_STRLEN_P(value));
    return true;
  }
  if( Z_TYPE_P(value) != IS_OBJECT ) {
    php_error(E_WARNING, "Invalid argument");
    return false;
  }
  if( Z_OBJCE_P(value) != MustacheTemplate_ce_ptr ) {
    php_error(E_WARNING, "Object not an instance of MustacheTemplate or MustacheAST");
    return false;
  }

  source = mustache_template_object_source(value);
  if( source.empty() ) {
    php_error(E_WARNING, "Empty MustacheTemplate");
    return false;
  }
  return true;
}

static bool mustache_partial_source(zval * value, std::string& source)
{
  value = mustache_dereference_zval(value);
  if( value != NULL && Z_TYPE_P(value) == IS_STRING ) {
    source.assign(Z_STRVAL_P(value), Z_STRLEN_P(value));
    return true;
  }
  if( value != NULL && Z_TYPE_P(value) == IS_OBJECT ) {
    if( Z_OBJCE_P(value) == MustacheTemplate_ce_ptr ) {
      source = mustache_template_object_source(value);
      return true;
    }
    php_error(E_WARNING, "Object not an instance of MustacheTemplate or MustacheAST");
    return false;
  }
  php_error(E_WARNING, "Partial array contains an invalid value");
  return false;
}

static std::unique_ptr<mustache::Node> mustache_clone_node(
    const mustache::Node& source, size_t depth, NodeCloneState& state)
{
  if( depth == 0 || depth > 64 ) {
    throw InvalidParameterException("MustacheAST nesting limit exceeded while cloning a partial");
  }
  if( state.nodes >= 100000 ) {
    throw InvalidParameterException("MustacheAST node limit exceeded while cloning a partial");
  }
  ++state.nodes;
  if( source.dataParts.size() > 256 ||
      state.dataParts > 100000 - source.dataParts.size() ) {
    throw InvalidParameterException("MustacheAST data-part limit exceeded while cloning a partial");
  }
  state.dataParts += source.dataParts.size();

  std::unique_ptr<mustache::Node> clone = std::make_unique<mustache::Node>();
  clone->type = source.type;
  clone->flags = source.flags;
  clone->data = source.data;
  clone->dataParts = source.dataParts;
  clone->startSequence = source.startSequence;
  clone->stopSequence = source.stopSequence;

  clone->children.reserve(source.children.size());
  for( const std::unique_ptr<mustache::Node>& child : source.children ) {
    if( child == NULL ) {
      throw InvalidParameterException("MustacheAST contains an empty child node");
    }
    clone->children.push_back(mustache_clone_node(*child, depth + 1, state));
  }
  if( source.child != NULL ) {
    clone->child = mustache_clone_node(*source.child, depth + 1, state);
  }
  for( const auto& partial : source.partials ) {
    if( partial.second == NULL ) {
      throw InvalidParameterException("MustacheAST contains an empty partial node");
    }
    clone->partials.emplace(
        partial.first, mustache_clone_node(*partial.second, depth + 1, state));
  }
  return clone;
}

static std::unique_ptr<mustache::Node> mustache_clone_node(const mustache::Node& source)
{
  NodeCloneState state;
  return mustache_clone_node(source, 1, state);
}

static bool mustache_partials_include_ast(zval * partials)
{
  partials = mustache_dereference_zval(partials);
  if( partials == NULL || Z_TYPE_P(partials) != IS_ARRAY ) {
    return false;
  }

  zval * value = NULL;
  ZEND_HASH_FOREACH_VAL_IND(Z_ARRVAL_P(partials), value) {
    if( mustache_is_ast(value) ) {
      return true;
    }
  } ZEND_HASH_FOREACH_END();
  return false;
}

static bool mustache_parse_template_param(zval * value, mustache::Mustache * mustache,
    mustache::Node& owned_node, const mustache::Node ** node)
{
  if( mustache_is_ast(value) ) {
    *node = mustache_ast_node(value);
    return *node != NULL;
  }

  std::string source;
  if( !mustache_template_source(value, source) ) {
    return false;
  }
  mustache->tokenize(std::string_view(source), &owned_node);
  *node = &owned_node;
  return true;
}

static bool mustache_compile_template_param(zval * value, mustache::Mustache * mustache,
    mustache::CompiledTemplate& compiled)
{
  std::string source;
  if( !mustache_template_source(value, source) ) {
    return false;
  }
  compiled = mustache->compile(std::string_view(source));
  return true;
}

static void mustache_compile_partials(zval * partials_value, mustache::Mustache * mustache,
    mustache::PartialMap& partials)
{
  partials_value = mustache_dereference_zval(partials_value);
  if( partials_value == NULL || Z_TYPE_P(partials_value) != IS_ARRAY ) {
    return;
  }

  zend_ulong numeric_key = 0;
  zend_string * key = NULL;
  zval * value = NULL;
  ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(partials_value), numeric_key, key, value) {
    (void) numeric_key;
    if( key == NULL ) {
      php_error(E_WARNING, "Partial array contains a non-string key");
      continue;
    }

    std::string source;
    if( !mustache_partial_source(value, source) ) {
      continue;
    }
    partials.emplace(
        std::string(ZSTR_VAL(key), ZSTR_LEN(key)),
        mustache->compile(std::string_view(source)));
  } ZEND_HASH_FOREACH_END();
}

static void mustache_parse_partials(zval * partials_value, mustache::Mustache * mustache,
    mustache::Node::Partials& partials)
{
  partials_value = mustache_dereference_zval(partials_value);
  if( partials_value == NULL || Z_TYPE_P(partials_value) != IS_ARRAY ) {
    return;
  }

  zend_ulong numeric_key = 0;
  zend_string * key = NULL;
  zval * value = NULL;
  ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(partials_value), numeric_key, key, value) {
    (void) numeric_key;
    if( key == NULL ) {
      php_error(E_WARNING, "Partial array contains a non-string key");
      continue;
    }

    std::unique_ptr<mustache::Node> partial;
    if( mustache_is_ast(value) ) {
      const mustache::Node * ast_node = mustache_ast_node(value);
      if( ast_node == NULL ) {
        continue;
      }
      // Node::Partials owns its values. Clone deliberately rather than
      // adopting the node still owned by the PHP MustacheAST object.
      partial = mustache_clone_node(*ast_node);
    } else {
      std::string source;
      if( !mustache_partial_source(value, source) ) {
        continue;
      }
      partial = std::make_unique<mustache::Node>();
      mustache->tokenize(std::string_view(source), partial.get());
    }
    partials.emplace(std::string(ZSTR_VAL(key), ZSTR_LEN(key)), std::move(partial));
  } ZEND_HASH_FOREACH_END();
}

#if defined(PHP_MUSTACHE_ARCHIVE_BENCHMARK)
static mustache::ArchivedTemplateLimits mustache_archive_benchmark_limits()
{
  mustache::ArchivedTemplateLimits limits;
  limits.maxInputBytes = size_t{16} * 1024 * 1024;
  limits.maxNestingDepth = 64;
  limits.maxNodes = 100000;
  limits.maxStringBytes = size_t{16} * 1024 * 1024;
  limits.maxDataPartsPerNode = 256;
  limits.maxDataParts = 100000;
  return limits;
}
#endif

} // namespace

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

/* {{{ proto boolean Mustache::setStartSequence(bool flag) */
PHP_METHOD(Mustache, setEscapeByDefault)
{
  try {
    // Custom parameters
    zend_long flag = 0;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Ol",
            &_this_zval, Mustache_ce_ptr, &flag) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);

    // Main
    payload->mustache->setEscapeByDefault((bool) flag != 0);
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
/* }}} Mustache::setStartSequence */

/* {{{ proto MustacheAST Mustache::parse(string template) */
PHP_METHOD(Mustache, parse)
{
  try {
    // Custom parameters
    zval * tmpl = NULL;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Oz",
            &_this_zval, Mustache_ce_ptr, &tmpl) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);

    // Check template parameter
    mustache::Node templateNode;
    const mustache::Node * templateNodePtr = NULL;
    if( !mustache_parse_template_param(tmpl, payload->mustache, templateNode, &templateNodePtr) ) {
      RETURN_FALSE;
      return;
    }

    // Handle return value
    zval * templateValue = mustache_dereference_zval(tmpl);
    if( templateValue != NULL && Z_TYPE_P(templateValue) == IS_STRING ) {
      if( MustacheAST_ce_ptr == NULL ) {
        php_error_docref(NULL, E_WARNING, "Class MustacheAST does not exist");
        RETURN_FALSE;
        return;
      }

      // Initialize new object
      object_init_ex(return_value, MustacheAST_ce_ptr);
      struct php_obj_MustacheAST * intern = php_mustache_ast_object_fetch_object(return_value);
      if( intern->state == NULL ) {
        throw InvalidParameterException("MustacheAST state was not initialized properly");
      }
      intern->state->node =
          std::make_unique<mustache::Node>(std::move(templateNode));

    // Ref - not sure if this is required
//    Z_SET_REFCOUNT_P(return_value, 1);
//    Z_SET_ISREF_P(return_value);

    } else if( templateValue != NULL && Z_TYPE_P(templateValue) == IS_OBJECT ) {
      // Handle return value for object parameter
      // @todo return the object itself?
      RETURN_TRUE;
    }

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::parse */

/* {{{ proto string Mustache::render(mixed template, array data, array partials) */
PHP_METHOD(Mustache, render)
{
  try {
    // Custom parameters
    zval * tmpl = NULL;
    zval * data = NULL;
    zval * partials = NULL;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Ozz|a/",
            &_this_zval, Mustache_ce_ptr, &tmpl, &data, &partials) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);

    // Prepare template data
    mustache::Data templateData;
    mustache::Data * templateDataPtr = &templateData;
    if( !mustache_parse_data_param(data, payload->mustache, &templateDataPtr) ) {
      RETURN_FALSE;
      return;
    }

    std::string output;
    if( mustache_is_ast(tmpl) || mustache_partials_include_ast(partials) ) {
      // AST-backed inputs remain on the compatibility renderer. Its partial
      // map owns explicit deep clones of public MustacheAST values.
      mustache::Node templateNode;
      const mustache::Node * templateNodePtr = NULL;
      if( !mustache_parse_template_param(tmpl, payload->mustache, templateNode, &templateNodePtr) ) {
        RETURN_FALSE;
        return;
      }
      mustache::Node::Partials templatePartials;
      mustache_parse_partials(partials, payload->mustache, templatePartials);
      zval * templateValue = mustache_dereference_zval(tmpl);
      if( templateValue != NULL && Z_TYPE_P(templateValue) == IS_STRING ) {
        output.reserve(Z_STRLEN_P(templateValue));
      }
      payload->mustache->render(
          templateNodePtr, templateDataPtr, &templatePartials, &output);
    } else {
      // Ordinary source templates and source-backed partials use immutable,
      // independently owned compiled handles.
      mustache::CompiledTemplate compiledTemplate;
      if( !mustache_compile_template_param(tmpl, payload->mustache, compiledTemplate) ) {
        RETURN_FALSE;
        return;
      }
      mustache::PartialMap compiledPartials;
      mustache_compile_partials(partials, payload->mustache, compiledPartials);
      output = payload->mustache->render(
          compiledTemplate, *templateDataPtr, compiledPartials);
    }

    // Output
    RETVAL_STRINGL(output.c_str(), output.length());

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::render */

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

    // Assign template to string
    std::string templateStr(template_str, template_len);

    // Tokenize template
    mustache::Node root;
    payload->mustache->tokenize(std::string_view(templateStr), &root);

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

#if defined(PHP_MUSTACHE_ARCHIVE_BENCHMARK)
/* {{{ proto string Mustache::benchmarkSerializeArchive(string template, array partials = []) */
PHP_METHOD(Mustache, benchmarkSerializeArchive)
{
  try {
    char * templateStr = NULL;
    size_t templateLen = 0;
    zval * partials = NULL;
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Os|a/",
            &_this_zval, Mustache_ce_ptr, &templateStr, &templateLen, &partials) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    mustache::Node root;
    payload->mustache->tokenize(std::string_view(templateStr, templateLen), &root);

    mustache::Node::Partials templatePartials;
    mustache_parse_partials(partials, payload->mustache, templatePartials);
    const std::vector<std::uint8_t> archive = mustache::serializeArchivedTemplate(
        root, templatePartials, mustache_archive_benchmark_limits());
    RETVAL_STRINGL(reinterpret_cast<const char *>(archive.data()), archive.size());
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::benchmarkSerializeArchive */

/* {{{ proto string Mustache::benchmarkRenderArchive(string archive, mixed data) */
PHP_METHOD(Mustache, benchmarkRenderArchive)
{
  try {
    char * archiveStr = NULL;
    size_t archiveLen = 0;
    zval * data = NULL;
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Osz",
            &_this_zval, Mustache_ce_ptr, &archiveStr, &archiveLen, &data) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    mustache::Data templateData;
    mustache::Data * templateDataPtr = &templateData;
    if( !mustache_parse_data_param(data, payload->mustache, &templateDataPtr) ) {
      RETURN_FALSE;
      return;
    }

    const mustache::ArchivedTemplateView archived = mustache::loadArchivedTemplate(
        std::string_view(archiveStr, archiveLen), mustache_archive_benchmark_limits());
    const std::string output = payload->mustache->render(archived, *templateDataPtr);
    RETVAL_STRINGL(output.c_str(), output.length());
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::benchmarkRenderArchive */
#endif
