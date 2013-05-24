
#include "mustache_mustache.hpp"
#include "mustache_data.hpp"
#include "mustache_template.hpp"



// Class Entries  --------------------------------------------------------------

zend_class_entry * Mustache_ce_ptr;



// Method Entries --------------------------------------------------------------

static zend_function_entry Mustache_methods[] = {
  PHP_ME(Mustache, __construct, Mustache____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(Mustache, getEscapeByDefault, Mustache__getEscapeByDefault_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, getStartSequence, Mustache__getStartSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, getStopSequence, Mustache__getStopSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setEscapeByDefault, Mustache__setEscapeByDefault_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setStartSequence, Mustache__setStartSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setStopSequence, Mustache__setStopSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, compile, Mustache__compile_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, tokenize, Mustache__tokenize_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, render, Mustache__render_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, debugDataStructure, Mustache__debugDataStructure_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};



// Object Handlers -------------------------------------------------------------

static zend_object_handlers Mustache_obj_handlers;

static void Mustache_obj_free(void *object TSRMLS_DC)
{
  try {
    php_obj_Mustache * payload = (php_obj_Mustache *)object;

    if( payload->mustache != NULL ) {
      delete payload->mustache;
    }
    
    efree(object);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}

static zend_object_value Mustache_obj_create(zend_class_entry *class_type TSRMLS_DC)
{
  zend_object_value retval;
  
  try {
    php_obj_Mustache * payload = NULL;
    zval * tmp = NULL;

    payload = (php_obj_Mustache *) emalloc(sizeof(php_obj_Mustache));
    memset(payload, 0, sizeof(php_obj_Mustache));
    payload->obj.ce = class_type;

    payload->mustache = new mustache::Mustache;
    
    // Set ini settings
    if( MUSTACHEG(default_escape_by_default) ) {
      payload->mustache->setEscapeByDefault(true);
    } else {
      payload->mustache->setEscapeByDefault(false);
    }
    if( MUSTACHEG(default_start_sequence) ) {
      payload->mustache->setStartSequence(MUSTACHEG(default_start_sequence), 0);
    }
    if( MUSTACHEG(default_stop_sequence) ) {
      payload->mustache->setStopSequence(MUSTACHEG(default_stop_sequence), 0);
    }
    
    retval.handle = zend_objects_store_put(payload, NULL, 
        (zend_objects_free_object_storage_t) Mustache_obj_free, NULL TSRMLS_CC);
    retval.handlers = &Mustache_obj_handlers;
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }

  return retval;
}



// MINIT -----------------------------------------------------------------------

PHP_MINIT_FUNCTION(mustache_mustache)
{
  try {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Mustache", Mustache_methods);
    ce.create_object = Mustache_obj_create;
    Mustache_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    memcpy(&Mustache_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    Mustache_obj_handlers.clone_obj = NULL;

    return SUCCESS;
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
    return FAILURE;
  }
}



// Methods ---------------------------------------------------------------------

/* {{{ proto void __construct()
   */
PHP_METHOD(Mustache, __construct)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_Mustache * payload = 
            (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} __construct */

/* {{{ proto boolean getEscapeByDefault()
   */
PHP_METHOD(Mustache, getEscapeByDefault)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_Mustache * payload = 
            (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Main
    if( payload->mustache->getEscapeByDefault() ) {
      RETURN_TRUE;
    } else {
      RETURN_FALSE;
    }
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} getEscapeByDefault */

/* {{{ proto string getStartSequence()
   */
PHP_METHOD(Mustache, getStartSequence)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_Mustache * payload = 
            (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Main
    const std::string & str = payload->mustache->getStartSequence();
    RETURN_STRING(str.c_str(), 1);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} getStartSequence */

/* {{{ proto string getStopSequence()
   */
PHP_METHOD(Mustache, getStopSequence)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", 
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_Mustache * payload = 
            (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Main
    const std::string & str = payload->mustache->getStopSequence();
    RETURN_STRING(str.c_str(), 1);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} getStopSequence */

/* {{{ proto boolean setStartSequence(bool flag)
   */
PHP_METHOD(Mustache, setEscapeByDefault)
{
  try {
    // Custom parameters
    long flag = 0;
  
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", 
            &_this_zval, Mustache_ce_ptr, &flag) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_Mustache * payload = 
            (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
    // Main
    payload->mustache->setEscapeByDefault((bool) flag != 0);
    RETURN_TRUE;
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} setEscapeByDefault */

/* {{{ proto boolean setStartSequence(string str)
   */
PHP_METHOD(Mustache, setStartSequence)
{
  try {
    // Custom parameters
    char * str = NULL;
    long str_len = 0;
    
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", 
            &_this_zval, Mustache_ce_ptr, &str, &str_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_Mustache * payload = 
            (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Main
    payload->mustache->setStartSequence(str/*, str_len*/);
    RETURN_TRUE;
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} setStartSequence */

/* {{{ proto boolean setStopSequence(string str)
   */
PHP_METHOD(Mustache, setStopSequence)
{
  try {
    // Custom parameters
    char * str = NULL;
    long str_len = 0;
    
    // Check parameters
    zval * _this_zval;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", 
            &_this_zval, Mustache_ce_ptr, &str, &str_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_Mustache * payload = 
            (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Main
    payload->mustache->setStopSequence(str/*, str_len*/);
    RETURN_TRUE;
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} setStartSequence */

/* {{{ proto MustacheTemplate compile(string template)
   */
PHP_METHOD(Mustache, compile)
{
  try {
    // Custom parameters
    zval * tmpl = NULL;
  
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oz", 
            &_this_zval, Mustache_ce_ptr, &tmpl) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_Mustache * payload = 
            (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Check template parameter
    mustache::Node * templateNodePtr = NULL;
    if( !mustache_parse_template_param(tmpl, payload->mustache, &templateNodePtr TSRMLS_CC) ) {
      delete templateNodePtr;
      RETURN_FALSE;
      return;
    }
    
    // Handle return value
    if( Z_TYPE_P(tmpl) == IS_STRING ) {
      // Get MustacheTemplate class entry
      std::string className("MustacheTemplate");
      zend_class_entry * MustacheTemplate_ce_ptr = 
              mustache_get_class_entry((char *)className.c_str(), className.length() TSRMLS_CC);

      if( MustacheTemplate_ce_ptr == NULL ) {
        delete templateNodePtr;
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Class %s does not exist%s", className.c_str());
        RETURN_FALSE;
        return;
      }
      
      // Initialize new object
      object_init_ex(return_value, MustacheTemplate_ce_ptr);
      php_obj_MustacheTemplate * intern = 
              (php_obj_MustacheTemplate *) zend_objects_get_address(return_value TSRMLS_CC);
      
      intern->node = templateNodePtr;
      
    // Ref - not sure if this is required
//    Z_SET_REFCOUNT_P(return_value, 1);
//    Z_SET_ISREF_P(return_value);
    
      
    } else if( Z_TYPE_P(tmpl) == IS_OBJECT ) {
      // Handle return value for object parameter
      // @todo return the object itself?
      RETURN_TRUE;
    }
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} compile */

/* {{{ proto array tokenize(string template)
   */
PHP_METHOD(Mustache, tokenize)
{
  try {
    // Custom parameters
    char * template_str = NULL;
    long template_len = 0;
  
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", 
            &_this_zval, Mustache_ce_ptr, &template_str, &template_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_Mustache * payload = 
            (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Assign template to string
    std::string templateStr(template_str/*, template_len*/);
    
    // Tokenize template
    mustache::Node root;
    payload->mustache->tokenize(&templateStr, &root);
    
    // Convert to PHP array
    mustache_node_to_zval(&root, return_value TSRMLS_CC);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} tokenize */

/* {{{ proto string render(mixed template, array data, array partials)
   */
PHP_METHOD(Mustache, render)
{
  try {
    // Custom parameters
    zval * tmpl = NULL;
    zval * data = NULL;
    zval * partials = NULL;
    
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ozz|a/", 
            &_this_zval, Mustache_ce_ptr, &tmpl, &data, &partials) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_Mustache * payload = 
            (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Prepare template tree
    mustache::Node templateNode;
    mustache::Node * templateNodePtr = &templateNode;
    if( !mustache_parse_template_param(tmpl, payload->mustache, &templateNodePtr TSRMLS_CC) ) {
      RETURN_FALSE;
      return;
    }
    
    // Prepare template data
    mustache::Data templateData;
    mustache::Data * templateDataPtr = &templateData;
    if( !mustache_parse_data_param(data, payload->mustache, &templateDataPtr TSRMLS_CC) ) {
      RETURN_FALSE;
      return;
    }
    
    // Tokenize partials
    mustache::Node::Partials templatePartials;
    mustache_parse_partials_param(partials, payload->mustache, &templatePartials TSRMLS_CC);
    
    // Render template
    std::string output;
    payload->mustache->render(templateNodePtr, templateDataPtr, &templatePartials, &output);
    
    // Output
    RETURN_STRING(output.c_str(), 1); // Yes reallocate
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} render */

/* {{{ proto array debugDataStructure(array data)
   */
PHP_METHOD(Mustache, debugDataStructure)
{
  try {
    // Custom parameters
    zval * data = NULL;
    
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oz", 
            &_this_zval, Mustache_ce_ptr, &data) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    php_obj_Mustache * payload = 
            (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
    
    // Prepare template data
    mustache::Data templateData;
    mustache_data_from_zval(&templateData, data TSRMLS_CC);

    // Reverse template data
    mustache_data_to_zval(&templateData, return_value TSRMLS_CC);
  
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} debugDataStructure */
