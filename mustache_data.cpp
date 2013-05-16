
#include "php_mustache.hpp"
#include "mustache_data.hpp"



// Class Entries  --------------------------------------------------------------

PHPAPI zend_class_entry * MustacheData_ce_ptr = NULL;



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
    zval * tmp;

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
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oz", 
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
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
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
