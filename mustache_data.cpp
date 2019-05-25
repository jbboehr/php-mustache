
#include <mustache/mustache.hpp>
#include <mustache/lambda.hpp>
#include "php_mustache.h"
#include <Zend/zend_closures.h>
#include "mustache_class_method_lambda.hpp"
#include "mustache_exceptions.hpp"
#include "mustache_lambda.hpp"
#include "mustache_zend_closure_lambda.hpp"
#include "mustache_data.hpp"

/* {{{ ZE2 OO definitions */
zend_class_entry * MustacheData_ce_ptr;
static zend_object_handlers MustacheData_obj_handlers;
/* }}} */

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
  MustacheData_obj_handlers.clone_obj = NULL;
  
  return SUCCESS;
}
/* }}} */

/* {{{ is_invokable_object */
static zend_always_inline bool is_invokable_object(const zend_class_entry * ce)
{
  const HashTable * function_table = ce != NULL ? &ce->function_table : NULL;
  return function_table != NULL && zend_hash_str_exists(function_table, "__invoke", strlen("__invoke"));
}
/* }}} is_invokable_object */

/* {{{ is_valid_function */
static zend_always_inline bool is_valid_function(const zend_function * f)
{
  return (f->common.fn_flags & ZEND_ACC_CTOR) == 0 &&
          (f->common.fn_flags & ZEND_ACC_DTOR) == 0 &&
          (f->common.fn_flags & ZEND_ACC_STATIC) == 0 &&
          (f->common.fn_flags & ZEND_ACC_PROTECTED) == 0 &&
          (f->common.fn_flags & ZEND_ACC_PRIVATE) == 0;
}
/* }}} */

/* {{{ is_valid_property */
static zend_always_inline bool is_valid_property(const zend_property_info * prop)
{
  return (prop->flags & ZEND_ACC_SHADOW) == 0 &&
          (prop->flags & ZEND_ACC_PROTECTED) == 0 &&
          (prop->flags & ZEND_ACC_PRIVATE) == 0;
}
/* }}} */

/* {{{ mustache_data_from_array_zval */
static zend_always_inline void mustache_data_from_array_zval(mustache::Data * node, zval * current)
{
  HashTable * data_hash = NULL;
  long data_count = 0;
  ulong key_nindex = 0;
  zend_string * key;
  std::string ckey;
  zval * data_entry = NULL;
  
  int length = 0;
  mustache::Data * child = NULL;

  node->type = mustache::Data::TypeNone;

  data_hash = HASH_OF(current);

#if PHP_VERSION_ID < 70300
  if( ZEND_HASH_APPLY_PROTECTION(data_hash) && ++data_hash->u.v.nApplyCount > 1 ) {
    php_error(E_WARNING, "Data includes circular reference");
    data_hash->u.v.nApplyCount--;
    return;
#else
  if (!(GC_FLAGS(data_hash) & GC_IMMUTABLE)) {
    if (GC_IS_RECURSIVE(data_hash)) {
      php_error(E_WARNING, "Data includes circular reference");
      return;
    }
    GC_PROTECT_RECURSION(data_hash);
#endif
  }

  data_count = zend_hash_num_elements(data_hash);
  ZEND_HASH_FOREACH_KEY_VAL_IND(data_hash, key_nindex, key, data_entry) {
    (void)key_nindex; /* avoid [-Wunused-but-set-variable] */
    if( !key ) {
      if( node->type == mustache::Data::TypeNone ) {
        node->init(mustache::Data::TypeArray, data_count);
      } else if( node->type != mustache::Data::TypeArray ) {
        php_error(E_WARNING, "Mixed numeric and associative arrays are not supported");
        return; // EXIT
      }
    } else {
      if( node->type == mustache::Data::TypeNone ) {
        node->type = mustache::Data::TypeMap;
      } else if( node->type != mustache::Data::TypeMap ) {
        php_error(E_WARNING, "Mixed numeric and associative arrays are not supported");
        return; // EXIT
      }
    }

    // Store value
    if( node->type == mustache::Data::TypeArray ) {
  	  child = new mustache::Data();
      mustache_data_from_zval(child, data_entry);
      node->array.push_back(child);
      node->length = ++length;
    } else if( node->type == mustache::Data::TypeMap ) {
      child = new mustache::Data;
      mustache_data_from_zval(child, data_entry);
      ckey.assign(ZSTR_VAL(key));
      node->data.insert(std::pair<std::string,mustache::Data*>(ckey, child));
    } else {
      php_error(E_WARNING, "Weird data conflict");
      // Whoops
    }
  } ZEND_HASH_FOREACH_END();

#if PHP_VERSION_ID < 70300
  if( ZEND_HASH_APPLY_PROTECTION(data_hash) ) {
    data_hash->u.v.nApplyCount--;
#else
  if (!(GC_FLAGS(data_hash) & GC_IMMUTABLE)) {
    GC_UNPROTECT_RECURSION(data_hash);
#endif
  }
}
/* }}} mustache_data_from_array_zval */

/* {{{ mustache_data_from_double_zval */
static zend_always_inline void mustache_data_from_double_zval(mustache::Data * node, zval * current)
{
  char * double_as_string;

  spprintf(&double_as_string, 0, "%.*G", (int) EG(precision), Z_DVAL_P(current));

  node->type = mustache::Data::TypeString;
  node->val = new std::string(double_as_string);

  efree(double_as_string);
}
/* }}} */

/* {{{ mustache_data_from_object_properties_zval */
static zend_always_inline void mustache_data_from_object_properties_zval(mustache::Data * node, zval * current)
{
  HashTable * data_hash = NULL;
  ulong key_nindex = 0;
  zend_string * key;
  std::string ckey;
  zval * data_entry = NULL;

  zend_class_entry * ce = Z_OBJCE_P(current);
  mustache::Data * child = NULL;

  zval * prop_zv;
  zend_property_info * prop;
  char * prop_name;
  const char * class_name;

  node->type = mustache::Data::TypeNone;

  if( Z_OBJ_HT_P(current)->get_properties != NULL ) {
    data_hash = Z_OBJ_HT_P(current)->get_properties(current);
  }
  if( data_hash != NULL && zend_hash_num_elements(data_hash) > 0 ) {
#if PHP_VERSION_ID < 70300
    if( ZEND_HASH_APPLY_PROTECTION(data_hash) && ++data_hash->u.v.nApplyCount > 1 ) {
      php_error(E_WARNING, "Data includes circular reference");
      data_hash->u.v.nApplyCount--;
      return;
#else
    if (!(GC_FLAGS(data_hash) & GC_IMMUTABLE)) {
      if (GC_IS_RECURSIVE(data_hash)) {
        php_error(E_WARNING, "Data includes circular reference");
        return;
      }
      GC_PROTECT_RECURSION(data_hash);
#endif
    }


    ZEND_HASH_FOREACH_KEY_VAL_IND(data_hash, key_nindex, key, data_entry) {
      (void)key_nindex; /* avoid [-Wunused-but-set-variable] */
      if( key && ZSTR_LEN(key) && ZSTR_VAL(key)[0] ) { // skip private/protected
        prop_name = ZSTR_VAL(key);

        // defined properties must be public
        // implicit properties won't be in properties_info, so they'll be assumed to be visible
        bool is_visible = true;
        if( ce != NULL && &ce->properties_info != NULL ) {
          prop_zv = zend_hash_find(&ce->properties_info, key);
          if( prop_zv != NULL ) {
            prop = (zend_property_info *) Z_PTR_P(prop_zv);
            is_visible = is_valid_property(prop);
            zend_unmangle_property_name(prop->name, &class_name, (const char **) &prop_name);
          }
        }

        if( is_visible ) {
          node->type = mustache::Data::TypeMap;

          child = new mustache::Data;
          mustache_data_from_zval(child, data_entry);
          ckey.assign(prop_name);
          node->data.insert(std::pair<std::string,mustache::Data*>(ckey, child));
        }
      }
    } ZEND_HASH_FOREACH_END();

#if PHP_VERSION_ID < 70300
    if( ZEND_HASH_APPLY_PROTECTION(data_hash) ) {
      data_hash->u.v.nApplyCount--;
#else
    if (!(GC_FLAGS(data_hash) & GC_IMMUTABLE)) {
      GC_UNPROTECT_RECURSION(data_hash);
#endif
    }
  }
}
/* }}} mustache_data_from_object_properties_zval */

/* {{{ mustache_data_from_object_functions_zval */
static zend_always_inline void mustache_data_from_object_functions_zval(mustache::Data * node, zval * current)
{
  HashTable * data_hash = NULL;
  ulong key_nindex = 0;
  zend_string * key;
  std::string ckey;
  zval * data_entry = NULL;
  zend_function * function_entry = NULL;

  zend_class_entry * ce = Z_OBJCE_P(current);
  mustache::Data * child = NULL;

  if( ce != NULL ) {
    data_hash = &ce->function_table;
  }
  if( data_hash != NULL && zend_hash_num_elements(data_hash) > 0 ) {
#if PHP_VERSION_ID < 70300
    if( ZEND_HASH_APPLY_PROTECTION(data_hash) && ++data_hash->u.v.nApplyCount > 1 ) {
      php_error(E_WARNING, "Data includes circular reference");
      data_hash->u.v.nApplyCount--;
      return;
#else
    if (!(GC_FLAGS(data_hash) & GC_IMMUTABLE)) {
      if (GC_IS_RECURSIVE(data_hash)) {
        php_error(E_WARNING, "Data includes circular reference");
        return;
      }
      GC_PROTECT_RECURSION(data_hash);
#endif
    }

    ZEND_HASH_FOREACH_KEY_VAL_IND(data_hash, key_nindex, key, data_entry) {
      function_entry = (zend_function *) Z_PTR_P(data_entry);
      (void)key; /* avoid [-Wunused-but-set-variable] */
      (void)key_nindex;
      if( is_valid_function(function_entry) ) {
        node->type = mustache::Data::TypeMap;

        ckey.assign(ZSTR_VAL(function_entry->common.function_name));

        child = new mustache::Data();
        child->type = mustache::Data::TypeLambda;
        child->lambda = new ClassMethodLambda(current, ZSTR_VAL(function_entry->common.function_name));

        node->data.insert(std::pair<std::string,mustache::Data*>(ckey,child));
      }
    } ZEND_HASH_FOREACH_END();

#if PHP_VERSION_ID < 70300
    if( ZEND_HASH_APPLY_PROTECTION(data_hash) ) {
      data_hash->u.v.nApplyCount--;
#else
    if (!(GC_FLAGS(data_hash) & GC_IMMUTABLE)) {
      GC_UNPROTECT_RECURSION(data_hash);
#endif
    }
  }
}
/* }}} mustache_data_from_object_functions_zval */

/* {{{ mustache_data_from_object_zval */
static zend_always_inline void mustache_data_from_object_zval(mustache::Data * node, zval * current)
{
  zend_class_entry * ce = Z_OBJCE_P(current);

  node->type = mustache::Data::TypeNone;

  if( ce == MustacheData_ce_ptr ) {
    // @todo
    php_error(E_WARNING, "MustacheData not implemented here");
  } else if( ce == zend_ce_closure ) {
    node->type = mustache::Data::TypeLambda;
    node->lambda = new ZendClosureLambda(current);
  } else if( is_invokable_object(ce) ) {
    node->type = mustache::Data::TypeLambda;
    node->lambda = new ClassMethodLambda(current, "__invoke");
  } else {
    // functions should take precendence over properties
    mustache_data_from_object_properties_zval(node, current);
    mustache_data_from_object_functions_zval(node, current);
  }
}
/* }}} mustache_data_from_object_zval */

/* {{{ mustache_data_from_zval */
void mustache_data_from_zval(mustache::Data * node, zval * current)
{
  if( Z_TYPE_P(current) == IS_INDIRECT ) {
    current = Z_INDIRECT_P(current);
  }
  ZVAL_DEREF(current);

  switch( Z_TYPE_P(current) ) {
      case IS_NULL:
          node->type = mustache::Data::TypeString;
          node->val = new std::string();
          break;
      case IS_LONG:
          node->type = mustache::Data::TypeString;
          node->val = new std::string(std::to_string((long long)Z_LVAL_P(current)));
          break;
      case IS_TRUE:
          node->type = mustache::Data::TypeString;
          node->val = new std::string("1");
          break;
      case IS_FALSE:
          node->type = mustache::Data::TypeString;
          node->val = new std::string();
          break;
      case IS_DOUBLE:
          mustache_data_from_double_zval(node, current);
          break;
      case IS_STRING:
          node->type = mustache::Data::TypeString;
          node->val = new std::string(Z_STRVAL_P(current)/*, (size_t) Z_STRLEN_P(current)*/);
          break;
      case IS_ARRAY:
          mustache_data_from_array_zval(node, current);
          break;
      case IS_OBJECT:
          mustache_data_from_object_zval(node, current);
          break;    
      default:
          php_error(E_WARNING, "Invalid data type: %d", Z_TYPE_P(current));
          break;
  }
}
/* }}} mustache_data_from_zval */

/* {{{ mustache_data_to_zval */
void mustache_data_to_zval(mustache::Data * node, zval * current)
{
  mustache::Data::Array::iterator a_it;
  mustache::Data::List::iterator l_it;
  mustache::Data::Map::iterator m_it;
  mustache::Data::Array childNode;
  int pos = 0;
  zval child = {0};

  switch( node->type ) {
    case mustache::Data::TypeNone:
      ZVAL_NULL(current);
      break;
    case mustache::Data::TypeString:
      ZVAL_STRINGL(current, node->val->c_str(), node->val->length());
      break;
    case mustache::Data::TypeArray:
        array_init(current);
        for( pos = 0; pos < node->length; pos++ ) {
            ZVAL_NULL(&child);
            mustache_data_to_zval(node->array[pos], &child);
            add_next_index_zval(current, &child);
        }
      break;
    case mustache::Data::TypeList:
      array_init(current);
      for ( l_it = node->children.begin() ; l_it != node->children.end(); l_it++ ) {
        ZVAL_NULL(&child);
        mustache_data_to_zval(*l_it, &child);
        add_next_index_zval(current, &child);
      }
      break;
    case mustache::Data::TypeMap:
      array_init(current);
      for ( m_it = node->data.begin() ; m_it != node->data.end(); m_it++ ) {
        ZVAL_NULL(&child);
        mustache_data_to_zval((*m_it).second, &child);
        add_assoc_zval(current, (*m_it).first.c_str(), &child);
      }
      break;
    default:
      ZVAL_NULL(current);
      php_error(E_WARNING, "Invalid data type");
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
    
    // Check if argument was given
    if( data == NULL ) {
      throw PhpInvalidParameterException();
    }
    
    // Convert data
    payload->data = new mustache::Data();
    mustache_data_from_zval(payload->data, data);
    
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
    mustache_data_to_zval(payload->data, return_value);
  
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} MustacheData::toValue */

