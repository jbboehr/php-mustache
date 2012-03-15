
#include "mustache_template.hpp"



// Method Entries --------------------------------------------------------------

static zend_function_entry MustacheTemplate_methods[] = {
  //PHP_ME(MustacheTemplate, __construct, MustacheTemplate____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheTemplate, render, MustacheTemplate__render_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};



// Object Handlers -------------------------------------------------------------

static zend_object_handlers MustacheTemplate_obj_handlers;

static void MustacheTemplate_obj_free(void *object TSRMLS_DC)
{
  php_obj_MustacheTemplate *payload = (php_obj_MustacheTemplate *)object;

  mustache::Node * node = payload->node;

  if( node != NULL ) {
    delete node;
  }

  efree(object);
}

static zend_object_value MustacheTemplate_obj_create_ex(zend_class_entry *class_type, mustache::Node * node TSRMLS_DC)
{
  php_obj_MustacheTemplate *payload;
  zval *tmp;
  zend_object_value retval;

  payload = (php_obj_MustacheTemplate *)emalloc(sizeof(php_obj_MustacheTemplate));
  memset(payload, 0, sizeof(php_obj_MustacheTemplate));
  payload->obj.ce = class_type;

  payload->node = node;

  retval.handle = zend_objects_store_put(payload, NULL, (zend_objects_free_object_storage_t) MustacheTemplate_obj_free, NULL TSRMLS_CC);
  retval.handlers = &MustacheTemplate_obj_handlers;

  return retval;
}

static zend_object_value MustacheTemplate_obj_create(zend_class_entry *class_type TSRMLS_DC)
{
  return MustacheTemplate_obj_create_ex(class_type, new mustache::Node);
}



// MINIT -----------------------------------------------------------------------

PHP_MINIT_FUNCTION(mustache_template)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "MustacheTemplate", MustacheTemplate_methods);
  ce.create_object = MustacheTemplate_obj_create;
  MustacheTemplate_ce_ptr = zend_register_internal_class(&ce);
  memcpy(&MustacheTemplate_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  MustacheTemplate_obj_handlers.clone_obj = NULL;
  
  return SUCCESS;
}



// Methods ---------------------------------------------------------------------

/* {{{ proto string render(array data, array partials)
   */
PHP_METHOD(MustacheTemplate, render)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_MustacheTemplate * payload;
  
  zval * data = NULL;
  zval * partials = NULL;
  
  mustache::Data templateData;
  
  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oa/|a/", &_this_zval, MustacheTemplate_ce_ptr, &data, &partials) == FAILURE) {
          return;
  }
  
  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_MustacheTemplate *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  RETURN_FALSE;
}
/* }}} render */
