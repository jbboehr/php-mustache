
#include "mustache_data.hpp"



// Method Entries --------------------------------------------------------------

static zend_function_entry MustacheData_methods[] = {
  PHP_ME(MustacheData, __construct, MustacheData____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheData, toValue, MustacheData__toValue_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};



// Object Handlers -------------------------------------------------------------

static zend_object_handlers MustacheData_obj_handlers;

static void MustacheData_obj_free(void *object TSRMLS_DC)
{
  php_obj_MustacheData * payload = (php_obj_MustacheData *) object;
  
  mustache::Data * data = payload->data;

  if( data != NULL ) {
    delete data;
  }

  efree(object);
}

static zend_object_value MustacheData_obj_create(zend_class_entry *class_type TSRMLS_DC)
{
  php_obj_MustacheData * payload;
  zval *tmp;
  zend_object_value retval;

  payload = (php_obj_MustacheData *) emalloc(sizeof(php_obj_MustacheData));
  memset(payload, 0, sizeof(php_obj_MustacheData));
  payload->obj.ce = class_type;

  try {
    payload->data = new mustache::Data;
  } catch( std::bad_alloc& e ) {
    payload->data = NULL;
    php_error(E_ERROR, "Failed to allocate memory when initializing MustacheData");
  }

  retval.handle = zend_objects_store_put(payload, NULL, (zend_objects_free_object_storage_t) MustacheData_obj_free, NULL TSRMLS_CC);
  retval.handlers = &MustacheData_obj_handlers;

  return retval;
}



// MINIT -----------------------------------------------------------------------

PHP_MINIT_FUNCTION(mustache_data)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "MustacheData", MustacheData_methods);
  ce.create_object = MustacheData_obj_create;
  MustacheData_ce_ptr = zend_register_internal_class(&ce);
  memcpy(&MustacheData_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  MustacheData_obj_handlers.clone_obj = NULL;
  
  return SUCCESS;
}



// Methods ---------------------------------------------------------------------

/* {{{ proto void __construct()
   */
PHP_METHOD(MustacheData, __construct)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_MustacheData * payload;
  
  zval * data;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|z", 
          &_this_zval, MustacheData_ce_ptr, &data) == FAILURE) {
    return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_MustacheData *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  // Main
  try {
    // Check if argument was given
    if( data == NULL ) {
      return;
    }
    
    // Convert data
    mustache_data_from_zval(payload->data, data TSRMLS_CC);
    
  } catch( mustache::Exception& e ) {
    mustache_error_handler(e.what(), &e, return_value TSRMLS_CC);
  } catch( std::bad_alloc& e ) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "Memory allocation failed");
  }
}
/* }}} __construct */

/* {{{ proto mixed toValue()
   */
PHP_METHOD(MustacheData, toValue)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_MustacheData * payload;
  
  zval * datacpy = NULL;
  
  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
          &_this_zval, MustacheData_ce_ptr) == FAILURE) {
    return;
  }
  
  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_MustacheData *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  // Main
  try {
    // Check if data was initialized
    if( payload->data == NULL ) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING, "MustacheData was not initialized properly");
      RETURN_FALSE;
      return;
    }
    
    // Reverse template data
    datacpy = mustache_data_to_zval(payload->data TSRMLS_CC);
    
    // Copy data into return value
    *return_value = *datacpy;
    zval_copy_ctor(return_value);
  
  } catch( mustache::Exception& e ) {
    mustache_error_handler(e.what(), &e, return_value TSRMLS_CC);
  } catch( std::bad_alloc& e ) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "Memory allocation failed");
  }
}
/* }}} toValue */
