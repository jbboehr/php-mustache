
#include "php_mustache.hpp"
#include "mustache_data.hpp"
#include "mustache_template.hpp"



// Class Entries  --------------------------------------------------------------

PHPAPI zend_class_entry * MustacheTemplate_ce_ptr = NULL;



// Method Entries --------------------------------------------------------------

static zend_function_entry MustacheTemplate_methods[] = {
  PHP_ME(MustacheTemplate, __construct, MustacheTemplate____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheTemplate, toArray, MustacheTemplate__toArray_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheTemplate, __toString, MustacheTemplate____toString_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};



// Object Handlers -------------------------------------------------------------

static zend_object_handlers MustacheTemplate_obj_handlers;

static void MustacheTemplate_obj_free(void *object TSRMLS_DC)
{
  try {
    php_obj_MustacheTemplate * payload = (php_obj_MustacheTemplate *) object;
    
    if( payload->tmpl != NULL ) {
      delete payload->tmpl;
    }
    if( payload->node != NULL ) {
      delete payload->node;
    }

    efree(object);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}

static zend_object_value MustacheTemplate_obj_create(zend_class_entry *class_type TSRMLS_DC)
{
  zend_object_value retval;
  
  try {
    php_obj_MustacheTemplate * payload;
    zval * tmp;

    payload = (php_obj_MustacheTemplate *) emalloc(sizeof(php_obj_MustacheTemplate));
    memset(payload, 0, sizeof(php_obj_MustacheTemplate));
    payload->obj.ce = class_type;
    
    payload->tmpl = NULL; //new std::string();
    payload->node = NULL; //new mustache::Node();

    retval.handle = zend_objects_store_put(payload, NULL, (zend_objects_free_object_storage_t) MustacheTemplate_obj_free, NULL TSRMLS_CC);
    retval.handlers = &MustacheTemplate_obj_handlers;
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
  
  return retval;
}



// MINIT -----------------------------------------------------------------------

PHP_MINIT_FUNCTION(mustache_template)
{
  try {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "MustacheTemplate", MustacheTemplate_methods);
    ce.create_object = MustacheTemplate_obj_create;
    MustacheTemplate_ce_ptr = zend_register_internal_class(&ce);
    memcpy(&MustacheTemplate_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    MustacheTemplate_obj_handlers.clone_obj = NULL;

    return SUCCESS;
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
    return FAILURE;
  }
}



// Methods ---------------------------------------------------------------------

/* {{{ proto void __construct(string tmpl)
   */
PHP_METHOD(MustacheTemplate, __construct)
{
  try {
    // Custom parameters
    char * template_str = NULL;
    long template_len = 0;
    
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", 
            &_this_zval, MustacheTemplate_ce_ptr, &template_str, &template_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_MustacheTemplate * payload = 
            (php_obj_MustacheTemplate *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Check if data was null
    if( template_len <= 0 || template_str == NULL ) {
      throw PhpInvalidParameterException();
    }
    
    // Store template
    payload->tmpl = new std::string(template_str/*, template_len*/);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} __construct */

/* {{{ proto array toArray()
   */
PHP_METHOD(MustacheTemplate, toArray)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
            &_this_zval, MustacheTemplate_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_MustacheTemplate * payload = 
            (php_obj_MustacheTemplate *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Check payload
    if( payload->node == NULL ) {
      throw InvalidParameterException("MustacheTemplate was not initialized properly");
    }
    
    // Convert to PHP array
    mustache_node_to_zval(payload->node, return_value TSRMLS_CC);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} toArray */

/* {{{ proto string __toString()
   */
PHP_METHOD(MustacheTemplate, __toString)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
            &_this_zval, MustacheTemplate_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_MustacheTemplate * payload = 
            (php_obj_MustacheTemplate *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Check payload
    if( payload->tmpl == NULL ) {
      throw InvalidParameterException("MustacheTemplate was not initialized properly");
    }
    
    // Return
    RETURN_STRING(payload->tmpl->c_str(), 1); // Yes reallocate
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} toArray */
