
#include "php_mustache.hpp"



// Class Entries ---------------------------------------------------------------

zend_class_entry * MustacheCode_ce_ptr;



// Object Handlers -------------------------------------------------------------

static zend_object_handlers MustacheCode_obj_handlers;

static void MustacheCode_obj_free(void *object TSRMLS_DC)
{
  try {
    php_obj_MustacheCode * payload = (php_obj_MustacheCode *) object;
    
    if( payload->codes != NULL ) {
      free(payload->codes);
    }
    
    zend_object_std_dtor((zend_object *)object TSRMLS_CC);

    efree(object);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}

static zend_object_value MustacheCode_obj_create(zend_class_entry *class_type TSRMLS_DC)
{
  zend_object_value retval;
  
  try {
    php_obj_MustacheCode * payload;
    zval * tmp;

    payload = (php_obj_MustacheCode *) emalloc(sizeof(php_obj_MustacheCode));
    memset(payload, 0, sizeof(php_obj_MustacheCode));
    
    zend_object_std_init((zend_object *) payload, class_type TSRMLS_CC);
    
#if PHP_VERSION_ID < 50399
    zend_hash_copy(payload->obj.properties, &(class_type->default_properties),
        (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval*));
#else
    object_properties_init(&payload->obj, class_type);
#endif
    
    retval.handle = zend_objects_store_put(payload, NULL, (zend_objects_free_object_storage_t) MustacheCode_obj_free, NULL TSRMLS_CC);
    retval.handlers = &MustacheCode_obj_handlers;
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
  
  return retval;
}



// Methods ---------------------------------------------------------------------

/* {{{ proto void __construct(string binaryString)
   */
PHP_METHOD(MustacheCode, __construct)
{
  try {
    // Custom parameters
    char * str = NULL;
    long str_len = 0;
    
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O|s", 
            &_this_zval, MustacheCode_ce_ptr, &str, &str_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_MustacheCode * payload = 
            (php_obj_MustacheCode *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Check payload
    if( payload->codes != NULL ) {
      throw InvalidParameterException("MustacheCode is already initialized");
    }
    
    // Copy
    if( str_len > 0 ) {
      payload->codes = (uint8_t *) malloc(sizeof(uint8_t) * str_len);
      memcpy(payload->codes, str, str_len);
      payload->length = str_len;
    }
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} __construct */

/* {{{ proto void __sleep()
   */
PHP_METHOD(MustacheCode, __sleep)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O", 
            &_this_zval, MustacheCode_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_MustacheCode * payload = 
            (php_obj_MustacheCode *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    array_init(return_value);
    
    // Check payload
    if( payload->codes != NULL ) {
      // Serialize and store
      if( payload->codes != NULL ) {
        zend_update_property_stringl(MustacheCode_ce_ptr, _this_zval, 
              ZEND_STRL("binaryString"), (const char *) payload->codes, 
              payload->length TSRMLS_CC);

        add_next_index_string(return_value, "binaryString", 1);
      }
    }
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} __sleep */

/* {{{ proto string toReadableString()
   */
PHP_METHOD(MustacheCode, toReadableString)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O", 
            &_this_zval, MustacheCode_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_MustacheCode * payload = 
            (php_obj_MustacheCode *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Check payload
    if( payload->codes == NULL ) {
      throw InvalidParameterException("MustacheCode was not initialized properly");
    }
    
    // Print codes
    std::string * output = mustache::Compiler::print(payload->codes, payload->length);
    
    // Copy
    if( output != NULL ) {
      int length = output->length();
      char * str = (char *) estrndup((char *) output->c_str(), length);
      delete output;
      RETURN_STRINGL(str, length, 0);
    }
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} toReadableString */

/* {{{ proto string __toString()
   */
PHP_METHOD(MustacheCode, __toString)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O", 
            &_this_zval, MustacheCode_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_MustacheCode * payload = 
            (php_obj_MustacheCode *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Check payload
    if( payload->codes == NULL ) {
      throw InvalidParameterException("MustacheCode was not initialized properly");
    }
    
    // Copy
    if( payload->length > 0 ) {
      char * str = (char *) estrndup((char *) payload->codes, payload->length);
      RETURN_STRINGL(str, payload->length, 0);
    }
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} __toString */


/* {{{ proto void __wakeup()
   */
PHP_METHOD(MustacheCode, __wakeup)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O", 
            &_this_zval, MustacheCode_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_MustacheCode * payload = 
            (php_obj_MustacheCode *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Get object properties
    // @todo should be able to convert this to use zend_hash_find
    int key_type = 0;
    char * key_str = NULL;
    uint key_len = 0;
    ulong key_nindex = 0;
    HashTable * data_hash = NULL;
    HashPosition data_pointer = NULL;
    zval **data_entry = NULL;
    long data_count = 0;
    if( Z_OBJ_HT_P(_this_zval)->get_properties != NULL ) {
      data_hash = Z_OBJ_HT_P(_this_zval)->get_properties(_this_zval TSRMLS_CC);
      data_count = zend_hash_num_elements(data_hash);
    }
    if( data_hash != NULL ) {
      char *prop_name, *class_name;
      zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
      while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
        if( zend_hash_get_current_key_ex(data_hash, &key_str, &key_len, 
                &key_nindex, false, &data_pointer) == HASH_KEY_IS_STRING ) {
#if PHP_API_VERSION >= 20100412
          zend_unmangle_property_name(key_str, key_len-1, (const char **) &class_name, (const char **) &prop_name);
#else
          zend_unmangle_property_name(key_str, key_len-1, &class_name, &prop_name);
#endif
          if( strcmp(prop_name, "binaryString") == 0 && Z_TYPE_PP(data_entry) == IS_STRING ) {
            if( payload->codes != NULL ) {
              free(payload->codes);
              payload->codes = NULL;
            }
            payload->length = Z_STRLEN_PP(data_entry);
            payload->codes = (uint8_t *) malloc(sizeof(uint8_t) * payload->length);
            memcpy(payload->codes, Z_STRVAL_PP(data_entry), payload->length);
          }
        }
        zend_hash_move_forward_ex(data_hash, &data_pointer);
      }
    }
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} __wakeup */



// Argument Info ---------------------------------------------------------------

ZEND_BEGIN_ARG_INFO_EX(MustacheCode____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, codeString)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheCode____sleep_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheCode__toReadableString_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheCode____toString_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheCode____wakeup_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()



// Method Entries --------------------------------------------------------------

static zend_function_entry MustacheCode_methods[] = {
  PHP_ME(MustacheCode, __construct, MustacheCode____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheCode, __sleep, MustacheCode____sleep_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheCode, toReadableString, MustacheCode__toReadableString_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheCode, __toString, MustacheCode____toString_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheCode, __wakeup, MustacheCode____wakeup_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};



// MINIT -----------------------------------------------------------------------

PHP_MINIT_FUNCTION(mustache_code)
{
  try {
    zend_class_entry ce;

    memcpy(&MustacheCode_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    MustacheCode_obj_handlers.clone_obj = NULL;
    
    INIT_CLASS_ENTRY(ce, "MustacheCode", MustacheCode_methods);
    ce.create_object = MustacheCode_obj_create;
    
    MustacheCode_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    MustacheCode_ce_ptr->create_object = MustacheCode_obj_create;
    
    zend_declare_property_null(MustacheCode_ce_ptr, ZEND_STRL("binaryString"), ZEND_ACC_PROTECTED TSRMLS_CC);
    
    return SUCCESS;
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
    return FAILURE;
  }
}
