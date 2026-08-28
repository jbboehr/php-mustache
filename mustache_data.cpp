
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mustache/mustache.hpp>
#include <mustache/lambda.hpp>
#include "php_mustache.h"
#include "mustache_private.hpp"
#include <Zend/zend_closures.h>
#include <Zend/zend_gc.h>
#include "mustache_class_method_lambda.hpp"
#include "mustache_exceptions.hpp"
#include "mustache_lambda.hpp"
#include "mustache_zend_closure_lambda.hpp"
#include "mustache_data.hpp"
#include <cmath>
#include <cstdint>
#include <memory>
#include <unordered_set>
#include <utility>

/* {{{ ZE2 OO definitions */
zend_class_entry * MustacheData_ce_ptr;
static zend_object_handlers MustacheData_obj_handlers;
/* }}} */

namespace {

void addGcValues(const mustache::Data& data, zend_get_gc_buffer * gc_buffer);

}

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(MustacheData____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheData__toValue_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ MustacheData_methods */
static zend_function_entry MustacheData_methods[] = {
  PHP_ME(MustacheData, __construct, MustacheData____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheData, toValue, MustacheData__toValue_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};
/* }}} */

/* {{{ php_mustache_data_object_fetch_object */
static inline struct php_obj_MustacheData * php_mustache_data_fetch_object(zend_object * obj)
{
  return (struct php_obj_MustacheData *)((char*)(obj) - XtOffsetOf(struct php_obj_MustacheData, std));
}

struct php_obj_MustacheData * php_mustache_data_object_fetch_object(zval * zv)
{
  return php_mustache_data_fetch_object(Z_OBJ_P(zv));
}
/* }}} */

/* {{{ MustacheData_obj_get_gc */
static HashTable * MustacheData_obj_get_gc(zend_object * object, zval ** table, int * n)
{
  struct php_obj_MustacheData * payload = php_mustache_data_fetch_object(object);
  zend_get_gc_buffer * gc_buffer = zend_get_gc_buffer_create();

  if( payload->data != NULL ) {
    addGcValues(*payload->data, gc_buffer);
  }
  zend_get_gc_buffer_use(gc_buffer, table, n);

  return zend_std_get_properties(object);
}
/* }}} */

/* {{{ MustacheData_obj_free */
static void MustacheData_obj_free(zend_object *object)
{
  try {
    struct php_obj_MustacheData * payload = php_mustache_data_fetch_object(object);

    if( payload->data != NULL ) {
      delete payload->data;
    }

    zend_object_std_dtor((zend_object *)object);
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} */

/* {{{ MustacheData_obj_create */
static zend_object * MustacheData_obj_create(zend_class_entry * ce)
{
  struct php_obj_MustacheData * intern;

  try {
    intern = (struct php_obj_MustacheData *) ecalloc(1, sizeof(struct php_obj_MustacheData) + zend_object_properties_size(ce));
    zend_object_std_init(&intern->std, ce);
    intern->std.handlers = &MustacheData_obj_handlers;
    return &intern->std;
  } catch(...) {
    mustache_exception_handler();
  }

  return NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mustache_data)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "MustacheData", MustacheData_methods);
  ce.create_object = MustacheData_obj_create;
  MustacheData_ce_ptr = zend_register_internal_class(&ce);
  memcpy(&MustacheData_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  MustacheData_obj_handlers.offset = XtOffsetOf(struct php_obj_MustacheData, std);
  MustacheData_obj_handlers.free_obj = MustacheData_obj_free;
  MustacheData_obj_handlers.get_gc = MustacheData_obj_get_gc;
  MustacheData_obj_handlers.clone_obj = NULL;

  return SUCCESS;
}
/* }}} */

namespace {

void addGcValues(const mustache::Data& data, zend_get_gc_buffer * gc_buffer)
{
  switch( data.type() ) {
    case mustache::Data::TypeList:
      for( const mustache::Data& value : data.listItems() ) {
        addGcValues(value, gc_buffer);
      }
      break;
    case mustache::Data::TypeMap:
      for( const auto& value : data.objectItems() ) {
        addGcValues(value.second, gc_buffer);
      }
      break;
    case mustache::Data::TypeArray:
      for( const mustache::Data& value : data.arrayItems() ) {
        addGcValues(value, gc_buffer);
      }
      break;
    case mustache::Data::TypeLambda: {
      Lambda * lambda = dynamic_cast<Lambda *>(data.lambdaValue());
      if( lambda != NULL ) {
        lambda->addGcValues(gc_buffer);
      }
      break;
    }
    default:
      break;
  }
}

struct DataConversionLimits {
    size_t maxNestingDepth = 32;
    size_t maxNodes = 100000;
    size_t maxStringBytes = size_t{64} * 1024 * 1024;
    size_t maxContainerEntries = 100000;
};

template <typename T>
class ActivePathGuard {
  private:
    std::unordered_set<const T *>& active;
    const T * value;

  public:
    ActivePathGuard(std::unordered_set<const T *>& active_values, const T * current) :
        active(active_values),
        value(current)
    {
      if( !active.insert(value).second ) {
        throw InvalidParameterException("Data includes circular reference");
      }
    }

    ~ActivePathGuard()
    {
      active.erase(value);
    }

    ActivePathGuard(const ActivePathGuard&) = delete;
    ActivePathGuard& operator=(const ActivePathGuard&) = delete;
};

static zend_always_inline bool is_invokable_object(const zend_class_entry * ce)
{
  const HashTable * function_table = ce != NULL ? &ce->function_table : NULL;
  return function_table != NULL && zend_hash_str_exists(function_table, ZEND_STRL("__invoke"));
}

static zend_always_inline bool is_valid_function(const zend_function * function)
{
  return (function->common.fn_flags & ZEND_ACC_STATIC) == 0 &&
#ifdef ZEND_ACC_CTOR
          (function->common.fn_flags & ZEND_ACC_CTOR) == 0 &&
#endif
#ifdef ZEND_ACC_DTOR
          (function->common.fn_flags & ZEND_ACC_DTOR) == 0 &&
#endif
          (function->common.fn_flags & ZEND_ACC_PROTECTED) == 0 &&
          (function->common.fn_flags & ZEND_ACC_PRIVATE) == 0;
}

static zend_always_inline bool is_valid_property(const zend_property_info * property)
{
  return (property->flags & ZEND_ACC_PROTECTED) == 0 &&
#ifdef ZEND_ACC_SHADOW
          (property->flags & ZEND_ACC_SHADOW) == 0 &&
#endif
          (property->flags & ZEND_ACC_PRIVATE) == 0;
}

class DataConverter {
  private:
    DataConversionLimits limits;
    size_t nodes = 0;
    size_t stringBytes = 0;
    size_t containerEntries = 0;
    std::unordered_set<const HashTable *> activeArrays;
    std::unordered_set<const zend_object *> activeObjects;
    std::unordered_set<const zend_reference *> activeReferences;

    [[noreturn]] void fail(const char * message) const
    {
      throw InvalidParameterException(message);
    }

    void consume(size_t& used, size_t amount, size_t maximum, const char * message)
    {
      if( used > maximum || amount > maximum - used ) {
        fail(message);
      }
      used += amount;
    }

    void addNode(size_t depth)
    {
      if( depth == 0 || depth > limits.maxNestingDepth ) {
        fail("Data nesting limit exceeded");
      }
      consume(nodes, 1, limits.maxNodes, "Data node count limit exceeded");
    }

    void addString(size_t length)
    {
      consume(stringBytes, length, limits.maxStringBytes, "Data string byte limit exceeded");
    }

    void addContainerEntry()
    {
      consume(containerEntries, 1, limits.maxContainerEntries, "Data container entry limit exceeded");
    }

    mustache::Data convertArray(zval * current, size_t depth)
    {
      HashTable * values_hash = Z_ARRVAL_P(current);
      ActivePathGuard<HashTable> active_guard(activeArrays, values_hash);
      mustache::Data::Array array_values;
      mustache::Data::Map object_values;
      bool saw_numeric_key = false;
      bool saw_string_key = false;
      zend_ulong numeric_key = 0;
      zend_string * string_key = NULL;
      zval * value = NULL;

      array_values.reserve(zend_hash_num_elements(values_hash));
      object_values.reserve(zend_hash_num_elements(values_hash));

      ZEND_HASH_FOREACH_KEY_VAL_IND(values_hash, numeric_key, string_key, value) {
        (void) numeric_key;
        if( string_key == NULL ) {
          saw_numeric_key = true;
        } else {
          saw_string_key = true;
        }
        if( saw_numeric_key && saw_string_key ) {
          fail("Mixed numeric and associative arrays are not supported");
        }

        addContainerEntry();
        if( string_key == NULL ) {
          array_values.push_back(convert(value, depth + 1));
        } else {
          addString(ZSTR_LEN(string_key));
          object_values.emplace(
              std::string(ZSTR_VAL(string_key), ZSTR_LEN(string_key)),
              convert(value, depth + 1));
        }
      } ZEND_HASH_FOREACH_END();

      if( saw_string_key ) {
        return mustache::Data::object(std::move(object_values));
      }
      return mustache::Data::array(std::move(array_values));
    }

    void addObjectProperties(mustache::Data::Map& values, zval * current, size_t depth)
    {
      HashTable * properties = NULL;
      zend_class_entry * class_entry = Z_OBJCE_P(current);
      zend_ulong numeric_key = 0;
      zend_string * key = NULL;
      zval * value = NULL;

      if( Z_OBJ_HT_P(current)->get_properties != NULL ) {
#if PHP_VERSION_ID >= 80000
        properties = Z_OBJ_HT_P(current)->get_properties(Z_OBJ_P(current));
#else
        properties = Z_OBJ_HT_P(current)->get_properties(current);
#endif
      }
      if( properties == NULL ) {
        return;
      }

      ZEND_HASH_FOREACH_KEY_VAL_IND(properties, numeric_key, key, value) {
        (void) numeric_key;
        if( key == NULL || ZSTR_LEN(key) == 0 || ZSTR_VAL(key)[0] == '\0' ) {
          continue;
        }

        const char * property_name = ZSTR_VAL(key);
        zend_string * property_name_source = key;
        bool is_visible = true;

        if( class_entry != NULL ) {
          zval * property_value = zend_hash_find(&class_entry->properties_info, key);
          if( property_value != NULL ) {
            zend_property_info * property = (zend_property_info *) Z_PTR_P(property_value);
            const char * declaring_class_name = NULL;
            is_visible = is_valid_property(property);
            if( zend_unmangle_property_name(property->name, &declaring_class_name, &property_name) == SUCCESS ) {
              property_name_source = property->name;
            }
          }
        }
        if( !is_visible ) {
          continue;
        }

        size_t property_name_offset =
            static_cast<size_t>(property_name - ZSTR_VAL(property_name_source));
        if( property_name_offset > ZSTR_LEN(property_name_source) ) {
          fail("Invalid object property name");
        }
        size_t property_name_length = ZSTR_LEN(property_name_source) - property_name_offset;

        addContainerEntry();
        addString(property_name_length);
        values.emplace(
            std::string(property_name, property_name_length),
            convert(value, depth + 1));
      } ZEND_HASH_FOREACH_END();
    }

    void addObjectFunctions(mustache::Data::Map& values, zval * current, size_t depth)
    {
      zend_class_entry * class_entry = Z_OBJCE_P(current);
      if( class_entry == NULL ) {
        return;
      }

      zval * function_value = NULL;
      ZEND_HASH_FOREACH_VAL_IND(&class_entry->function_table, function_value) {
        zend_function * function = (zend_function *) Z_PTR_P(function_value);
        zend_string * function_name = function->common.function_name;
        if( !is_valid_function(function) || function_name == NULL ) {
          continue;
        }

        std::string key(ZSTR_VAL(function_name), ZSTR_LEN(function_name));
        if( values.find(key) != values.end() ) {
          continue;
        }

        addContainerEntry();
        addNode(depth + 1);
        addString(key.length());
        values.emplace(
            std::move(key),
            mustache::Data::lambda(std::make_unique<ClassMethodLambda>(
                current, ZSTR_VAL(function_name), ZSTR_LEN(function_name))));
      } ZEND_HASH_FOREACH_END();
    }

    mustache::Data convertObject(zval * current, size_t depth)
    {
      zend_class_entry * class_entry = Z_OBJCE_P(current);
      if( class_entry == MustacheData_ce_ptr ) {
        fail("Nested MustacheData values are not supported");
      }
      if( class_entry == zend_ce_closure ) {
        return mustache::Data::lambda(std::make_unique<ZendClosureLambda>(current));
      }
      if( is_invokable_object(class_entry) ) {
        return mustache::Data::lambda(
            std::make_unique<ClassMethodLambda>(current, ZEND_STRL("__invoke")));
      }

      ActivePathGuard<zend_object> active_guard(activeObjects, Z_OBJ_P(current));
      mustache::Data::Map values;
      addObjectProperties(values, current, depth);
      addObjectFunctions(values, current, depth);
      return mustache::Data::object(std::move(values));
    }

  public:
    mustache::Data convert(zval * current, size_t depth = 1)
    {
      if( current == NULL ) {
        fail("Missing data value");
      }
      if( Z_TYPE_P(current) == IS_INDIRECT ) {
        return convert(Z_INDIRECT_P(current), depth);
      }
      if( Z_TYPE_P(current) == IS_REFERENCE ) {
        zend_reference * reference = Z_REF_P(current);
        ActivePathGuard<zend_reference> active_guard(activeReferences, reference);
        return convert(Z_REFVAL_P(current), depth);
      }

      addNode(depth);
      switch( Z_TYPE_P(current) ) {
        case IS_NULL:
          return mustache::Data::null();
        case IS_FALSE:
          return mustache::Data::boolean(false);
        case IS_TRUE:
          return mustache::Data::boolean(true);
        case IS_LONG:
          return mustache::Data::integer(static_cast<std::int64_t>(Z_LVAL_P(current)));
        case IS_DOUBLE:
          if( !std::isfinite(Z_DVAL_P(current)) ) {
            fail("Non-finite floating-point data is not supported");
          }
          return mustache::Data::floating(Z_DVAL_P(current));
        case IS_STRING:
          addString(Z_STRLEN_P(current));
          return mustache::Data::string(
              std::string(Z_STRVAL_P(current), Z_STRLEN_P(current)));
        case IS_ARRAY:
          return convertArray(current, depth);
        case IS_OBJECT:
          return convertObject(current, depth);
        default:
          fail("Invalid data type");
      }
    }
};

} // namespace

/* {{{ mustache_data_from_zval */
mustache::Data mustache_data_from_zval(zval * current)
{
  DataConverter converter;
  return converter.convert(current);
}
/* }}} mustache_data_from_zval */

/* {{{ mustache_data_to_zval */
void mustache_data_to_zval(const mustache::Data& node, zval * current)
{
  switch( node.type() ) {
    case mustache::Data::TypeNone:
      ZVAL_NULL(current);
      break;
    case mustache::Data::TypeString: {
      const std::string& value = node.stringValue();
      ZVAL_STRINGL(current, value.c_str(), value.length());
      break;
    }
    case mustache::Data::TypeBoolean:
      ZVAL_BOOL(current, node.booleanValue());
      break;
    case mustache::Data::TypeInteger:
      ZVAL_LONG(current, static_cast<zend_long>(node.integerValue()));
      break;
    case mustache::Data::TypeDouble:
      ZVAL_DOUBLE(current, node.floatingValue());
      break;
    case mustache::Data::TypeArray:
      array_init_size(current, node.arrayItems().size());
      for( const mustache::Data& value : node.arrayItems() ) {
        zval child;
        mustache_data_to_zval(value, &child);
        add_next_index_zval(current, &child);
      }
      break;
    case mustache::Data::TypeList:
      array_init_size(current, node.listItems().size());
      for( const mustache::Data& value : node.listItems() ) {
        zval child;
        mustache_data_to_zval(value, &child);
        add_next_index_zval(current, &child);
      }
      break;
    case mustache::Data::TypeMap:
      array_init_size(current, node.objectItems().size());
      for( const auto& value : node.objectItems() ) {
        zval child;
        mustache_data_to_zval(value.second, &child);
        add_assoc_zval_ex(current, value.first.c_str(), value.first.length(), &child);
      }
      break;
    case mustache::Data::TypeLambda:
      ZVAL_NULL(current);
      php_error(E_WARNING, "Lambda data cannot be converted to a PHP value");
      break;
  }
}
/* }}} mustache_data_to_zval */

/* {{{ proto void MustacheData::__construct() */
PHP_METHOD(MustacheData, __construct)
{
  try {
    // Custom parameters
    zval * data = NULL;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Oz",
            &_this_zval, MustacheData_ce_ptr, &data) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_MustacheData * payload = php_mustache_data_object_fetch_object(_this_zval);

    if( payload->data != NULL ) {
      throw InvalidParameterException("MustacheData is already initialized");
    }

    // Check if argument was given
    if( data == NULL ) {
      throw PhpInvalidParameterException();
    }

    // Convert data
    std::unique_ptr<mustache::Data> converted =
        std::make_unique<mustache::Data>(mustache_data_from_zval(data));
    payload->data = converted.release();

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheData::__construct */

/* {{{ proto mixed MustacheData::toValue() */
PHP_METHOD(MustacheData, toValue)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, MustacheData_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    _this_zval = getThis();
    struct php_obj_MustacheData * payload = php_mustache_data_object_fetch_object(_this_zval);

    // Check if data was initialized
    if( payload->data == NULL ) {
      php_error_docref(NULL, E_WARNING, "MustacheData was not initialized properly");
      RETURN_FALSE;
      return;
    }

    // Reverse template data
    mustache_data_to_zval(*payload->data, return_value);

  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheData::toValue */
