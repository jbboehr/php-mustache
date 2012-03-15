
#include "mustache_template.hpp"

#include "mustache_data.hpp"



// Method Entries --------------------------------------------------------------

static zend_function_entry MustacheTemplate_methods[] = {
  PHP_ME(MustacheTemplate, __construct, MustacheTemplate____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheTemplate, render, MustacheTemplate__render_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheTemplate, toArray, MustacheTemplate__toArray_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheTemplate, __toString, MustacheTemplate____toString_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};



// Object Handlers -------------------------------------------------------------

static zend_object_handlers MustacheTemplate_obj_handlers;

static void MustacheTemplate_obj_free(void *object TSRMLS_DC)
{
  php_obj_MustacheTemplate *payload = (php_obj_MustacheTemplate *)object;

  mustache::Mustache * mustache = payload->mustache;
  std::string * tmpl = payload->tmpl;
  mustache::Node * node = payload->node;

  if( node != NULL ) {
    delete node;
  }
  if( tmpl != NULL ) {
    delete tmpl;
  }
  if( mustache != NULL ) {
    delete mustache;
  }

  efree(object);
}

static zend_object_value MustacheTemplate_obj_create(zend_class_entry *class_type TSRMLS_DC)
{
  php_obj_MustacheTemplate *payload;
  zval *tmp;
  zend_object_value retval;

  payload = (php_obj_MustacheTemplate *)emalloc(sizeof(php_obj_MustacheTemplate));
  memset(payload, 0, sizeof(php_obj_MustacheTemplate));
  payload->obj.ce = class_type;

  payload->mustache = new mustache::Mustache;
  payload->tmpl = new std::string;
  payload->node = new mustache::Node;

  retval.handle = zend_objects_store_put(payload, NULL, (zend_objects_free_object_storage_t) MustacheTemplate_obj_free, NULL TSRMLS_CC);
  retval.handlers = &MustacheTemplate_obj_handlers;

  return retval;
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

/* {{{ proto void __construct(string tmpl)
   */
PHP_METHOD(MustacheTemplate, __construct)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_MustacheTemplate *payload;
  
  char * template_str;
  long template_len;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|s", &_this_zval, MustacheTemplate_ce_ptr, &template_str, &template_len) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_MustacheTemplate *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  if( template_len <= 0 ) {
    return;
  }
  
  // Main
  try {
    // Store template
    payload->tmpl->assign(template_str);
    
    // Tokenize template
    payload->mustache->tokenize(payload->tmpl, payload->node);
    
  } catch( mustache::Exception& e ) {
    
    php_error(E_WARNING, (char *) e.what());
    RETURN_FALSE;
    
  }
}
/* }}} __construct */

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
  mustache::Data * templateDataPtr;
  mustache::Node::Partials templatePartials;
  std::string output;
  
  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|za/", &_this_zval, MustacheTemplate_ce_ptr, &data, &partials) == FAILURE) {
          return;
  }
  
  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_MustacheTemplate *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  if( payload->mustache == NULL || payload->node == NULL ) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "MustacheTemplate was not initialized properly");
    RETURN_FALSE;
    return;
  }
  
  if( Z_TYPE_P(data) == IS_OBJECT ) {
    zend_class_entry * tmp_ce = Z_OBJCE_P(data);
    std::string className("MustacheData");
    zend_class_entry * mtce = mustache_get_class_entry((char *)className.c_str(), className.length());
    if( tmp_ce == NULL || mtce == NULL || tmp_ce != mtce ) {
      php_error(E_WARNING, "Object not an instance of MustacheData");
      RETURN_FALSE;
      return;
    } else {
      php_obj_MustacheData * dataPayload = (php_obj_MustacheData *) zend_object_store_get_object(data TSRMLS_CC);
      templateDataPtr = dataPayload->data;
    }
  } else {
    mustache_data_from_zval(&templateData, data);
    templateDataPtr = &templateData;
  }
  
  try {
    
    // Tokenize partials
    mustache_partials_from_zval(payload->mustache, &templatePartials, partials);
    
    // Render template
    payload->mustache->render(payload->node, templateDataPtr, &templatePartials, &output);
    
    // Output
    RETURN_STRING(output.c_str(), 1); // Yes reallocate
    
  } catch( mustache::Exception& e ) {
    
    php_error(E_WARNING, (char *) e.what());
    RETURN_FALSE;
    
  }
}
/* }}} render */

/* {{{ proto array toArray()
   */
PHP_METHOD(MustacheTemplate, toArray)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_MustacheTemplate * payload;
  
  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, MustacheTemplate_ce_ptr) == FAILURE) {
          return;
  }
  
  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_MustacheTemplate *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  if( payload->mustache == NULL || payload->node == NULL ) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "MustacheTemplate was not initialized properly");
    RETURN_FALSE;
    return;
  }
  
  // Tokenize template
  try {
    
    // Convert to PHP array
    mustache_node_to_zval(payload->node, return_value);
    
  } catch( mustache::Exception& e ) {
    
    php_error(E_WARNING, (char *) e.what());
    RETURN_FALSE;
    
  }
}
/* }}} toArray */

/* {{{ proto string __toString()
   */
PHP_METHOD(MustacheTemplate, __toString)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_MustacheTemplate * payload;
  
  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, MustacheTemplate_ce_ptr) == FAILURE) {
          return;
  }
  
  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_MustacheTemplate *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  if( payload->mustache == NULL || payload->node == NULL ) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "MustacheTemplate was not initialized properly");
    RETURN_FALSE;
    return;
  }
  
  // Main
  try {
    
    RETURN_STRING(payload->tmpl->c_str(), 1); // Yes reallocate
    
  } catch( mustache::Exception& e ) {
    
    php_error(E_WARNING, (char *) e.what());
    RETURN_FALSE;
    
  }
}
/* }}} toArray */