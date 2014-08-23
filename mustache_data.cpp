
#include "php_mustache.hpp"



// Declarations ----------------------------------------------------------------

PHP_METHOD(MustacheData, __construct);
PHP_METHOD(MustacheData, toValue);



// Argument Info ---------------------------------------------------------------

ZEND_BEGIN_ARG_INFO_EX(MustacheData____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()
        
ZEND_BEGIN_ARG_INFO_EX(MustacheData__toValue_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()



// Class Entries ---------------------------------------------------------------

zend_class_entry * MustacheData_ce_ptr;



// Method Entries --------------------------------------------------------------

static zend_function_entry MustacheData_methods[] = {
  PHP_ME(MustacheData, __construct, MustacheData____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheData, toValue, MustacheData__toValue_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};



// Utility ---------------------------------------------------------------------

void mustache_data_from_zval(mustache::Data * node, zval * current TSRMLS_DC)
{
  HashTable * data_hash = NULL;
  HashPosition data_pointer = NULL;
  zval **data_entry = NULL;
  long data_count = 0;
  
  int key_type = 0;
  char * key_str = NULL;
  uint key_len = 0;
  ulong key_nindex = 0;
  std::string ckey;
  
  int ArrayPos = 0;
  mustache::Data * child = NULL;
  zend_class_entry * ce = NULL;
  
  switch( Z_TYPE_P(current) ) {
      case IS_NULL:
      case IS_LONG:
      case IS_BOOL:
      case IS_DOUBLE:
      case IS_STRING:
        convert_to_string(current);
        node->type = mustache::Data::TypeString;
        node->val = new std::string(Z_STRVAL_P(current)/*, (size_t) Z_STRLEN_P(current)*/);
        break;
      case IS_ARRAY: // START IS_ARRAY -----------------------------------------
        node->type = mustache::Data::TypeNone;
        
        data_hash = HASH_OF(current);
        data_count = zend_hash_num_elements(data_hash);
        zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
        while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
          // Get current key
          key_type = zend_hash_get_current_key_ex(data_hash, &key_str, &key_len, &key_nindex, false, &data_pointer);
          // Check key type
          if( key_type == HASH_KEY_IS_LONG ) {
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
            mustache_data_from_zval(child, *data_entry TSRMLS_CC);
            node->array[ArrayPos++] = child;
            node->length = ArrayPos;
          } else if( node->type == mustache::Data::TypeMap ) {
            child = new mustache::Data;
            mustache_data_from_zval(child, *data_entry TSRMLS_CC);
            ckey.assign(key_str);
            node->data.insert(std::pair<std::string,mustache::Data*>(ckey,child));
          } else {
            php_error(E_WARNING, "Weird data conflict");
            // Whoops
          }
          zend_hash_move_forward_ex(data_hash, &data_pointer);
        }
  
        break; // END IS_ARRAY -------------------------------------------------
    case IS_OBJECT:
      node->type = mustache::Data::TypeNone;
      
      if( Z_OBJCE_P(current) == MustacheData_ce_ptr ) {
        // @todo
        php_error(E_WARNING, "MustacheData not implemented here");
      } else {
        // Get object properties
        if( Z_OBJ_HT_P(current)->get_properties != NULL ) {
          data_hash = Z_OBJ_HT_P(current)->get_properties(current TSRMLS_CC);
          data_count = zend_hash_num_elements(data_hash);
        }
        if( data_hash != NULL ) {
          char *prop_name, *class_name;
          node->type = mustache::Data::TypeMap;
          zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
          while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
            if( zend_hash_get_current_key_ex(data_hash, &key_str, &key_len, 
                    &key_nindex, false, &data_pointer) == HASH_KEY_IS_STRING ) {
#if PHP_API_VERSION >= 20100412
              zend_unmangle_property_name(key_str, key_len-1, (const char **) &class_name, (const char **) &prop_name);
#else
              zend_unmangle_property_name(key_str, key_len-1, &class_name, &prop_name);
#endif
              child = new mustache::Data;
              mustache_data_from_zval(child, *data_entry TSRMLS_CC);
              ckey.assign(prop_name);
              node->data.insert(std::pair<std::string,mustache::Data*>(ckey,child));
            }
            zend_hash_move_forward_ex(data_hash, &data_pointer);
          }
        }
      }
      break;
    default:
      php_error(E_WARNING, "Invalid data type: %d", Z_TYPE_P(current));
      break;
  }
}

void mustache_data_to_zval(mustache::Data * node, zval * current TSRMLS_DC)
{
  mustache::Data::Array::iterator a_it;
  mustache::Data::List::iterator l_it;
  mustache::Data::Map::iterator m_it;
  mustache::Data::Array childNode;
  int pos = 0;
  
  switch( node->type ) {
    case mustache::Data::TypeNone:
      Z_TYPE_P(current) = IS_NULL;
      break;
    case mustache::Data::TypeString:
      Z_TYPE_P(current) = IS_STRING;
      Z_STRVAL_P(current) = (char *) estrdup(node->val->c_str());
      Z_STRLEN_P(current) = node->val->length();
      break;
    case mustache::Data::TypeArray:
        array_init(current);
        for( pos = 0; pos < node->length; pos++ ) {
            zval * child = NULL;
            ALLOC_INIT_ZVAL(child);
            mustache_data_to_zval(node->array[pos], child TSRMLS_CC);
            add_next_index_zval(current, child);
        }
        /*
        for ( a_it = node->array.begin() ; a_it != node->array.end(); a_it++ ) {
        }
      array_init(current);
      childNode = node->array;
      for( pos = 0; pos < node->length; pos++, childNode++ ) {
        zval * child = NULL;
        ALLOC_INIT_ZVAL(child);
        mustache_data_to_zval(childNode, child TSRMLS_CC);
        add_next_index_zval(current, child);
      }
      */
      break;
    case mustache::Data::TypeList:
      array_init(current);
      for ( l_it = node->children.begin() ; l_it != node->children.end(); l_it++ ) {
        zval * child = NULL;
        ALLOC_INIT_ZVAL(child);
        mustache_data_to_zval(*l_it, child TSRMLS_CC);
        add_next_index_zval(current, child);
      }
      break;
    case mustache::Data::TypeMap:
      array_init(current);
      for ( m_it = node->data.begin() ; m_it != node->data.end(); m_it++ ) {
        zval * child = NULL;
        ALLOC_INIT_ZVAL(child);
        mustache_data_to_zval((*m_it).second, child TSRMLS_CC);
        add_assoc_zval(current, (*m_it).first.c_str(), child);
      }
      break;
    default:
      Z_TYPE_P(current) = IS_NULL;
      php_error(E_WARNING, "Invalid data type");
      break;
  }
}



// Object Handlers -------------------------------------------------------------

static zend_object_handlers MustacheData_obj_handlers;

static void MustacheData_obj_free(void *object TSRMLS_DC)
{
  try {
    php_obj_MustacheData * payload = (php_obj_MustacheData *) object;
    
    if( payload->data != NULL ) {
      delete payload->data;
    }

    efree(object);
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}

static zend_object_value MustacheData_obj_create(zend_class_entry *class_type TSRMLS_DC)
{
  zend_object_value retval;
  
  try {
    php_obj_MustacheData * payload;

    payload = (php_obj_MustacheData *) emalloc(sizeof(php_obj_MustacheData));
    memset(payload, 0, sizeof(php_obj_MustacheData));
    payload->obj.ce = class_type;

    payload->data = NULL;

    retval.handle = zend_objects_store_put(payload, NULL, (zend_objects_free_object_storage_t) MustacheData_obj_free, NULL TSRMLS_CC);
    retval.handlers = &MustacheData_obj_handlers;

  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }

  return retval;
}



// MINIT -----------------------------------------------------------------------

PHP_MINIT_FUNCTION(mustache_data)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "MustacheData", MustacheData_methods);
  ce.create_object = MustacheData_obj_create;
  MustacheData_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
  memcpy(&MustacheData_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  MustacheData_obj_handlers.clone_obj = NULL;
  
  return SUCCESS;
}



// Methods ---------------------------------------------------------------------

/* {{{ proto void __construct()
   */
PHP_METHOD(MustacheData, __construct)
{
  try {
    // Custom parameters
    zval * data = NULL;

    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "Oz", 
            &_this_zval, MustacheData_ce_ptr, &data) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_MustacheData * payload = 
            (php_obj_MustacheData *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Check if argument was given
    if( data == NULL ) {
      throw PhpInvalidParameterException();
    }
    
    // Convert data
    payload->data = new mustache::Data();
    mustache_data_from_zval(payload->data, data TSRMLS_CC);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} __construct */

/* {{{ proto mixed toValue()
   */
PHP_METHOD(MustacheData, toValue)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O", 
            &_this_zval, MustacheData_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_MustacheData * payload = 
            (php_obj_MustacheData *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
    // Check if data was initialized
    if( payload->data == NULL ) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING, "MustacheData was not initialized properly");
      RETURN_FALSE;
      return;
    }
    
    // Reverse template data
    mustache_data_to_zval(payload->data, return_value TSRMLS_CC);
  
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} toValue */
