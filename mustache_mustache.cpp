
#include "mustache_mustache.hpp"
#include "mustache_template.hpp"



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
  php_obj_Mustache *payload = (php_obj_Mustache *)object;

  mustache::Mustache * mustache = payload->mustache;

  delete mustache;

  efree(object);
}

static zend_object_value Mustache_obj_create(zend_class_entry *class_type TSRMLS_DC)
{
  php_obj_Mustache *payload;
  zval *tmp;
  zend_object_value retval;

  payload = (php_obj_Mustache *)emalloc(sizeof(php_obj_Mustache));
  memset(payload, 0, sizeof(php_obj_Mustache));
  payload->obj.ce = class_type;

  payload->mustache = new mustache::Mustache;

  retval.handle = zend_objects_store_put(payload, NULL, (zend_objects_free_object_storage_t) Mustache_obj_free, NULL TSRMLS_CC);
  retval.handlers = &Mustache_obj_handlers;

  return retval;
}



// MINIT -----------------------------------------------------------------------

PHP_MINIT_FUNCTION(mustache_mustache)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "Mustache", Mustache_methods);
  ce.create_object = Mustache_obj_create;
  Mustache_ce_ptr = zend_register_internal_class(&ce);
  memcpy(&Mustache_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  Mustache_obj_handlers.clone_obj = NULL;
  
  return SUCCESS;
}



// Methods ---------------------------------------------------------------------

/* {{{ proto void __construct()
   */
PHP_METHOD(Mustache, __construct)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, Mustache_ce_ptr) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
}
/* }}} __construct */

/* {{{ proto bool getEscapeByDefault()
   */
PHP_METHOD(Mustache, getEscapeByDefault)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, Mustache_ce_ptr) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  if( payload->mustache->getEscapeByDefault() ) {
    RETURN_TRUE;
  } else {
    RETURN_FALSE;
  }
}
/* }}} getEscapeByDefault */

/* {{{ proto string getStartSequence()
   */
PHP_METHOD(Mustache, getStartSequence)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, Mustache_ce_ptr) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  const std::string & str = payload->mustache->getStartSequence();
  
  RETURN_STRING(str.c_str(), 1);
}
/* }}} getStartSequence */

/* {{{ proto string getStopSequence()
   */
PHP_METHOD(Mustache, getStopSequence)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, Mustache_ce_ptr) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  const std::string & str = payload->mustache->getStopSequence();
  
  RETURN_STRING(str.c_str(), 1);
}
/* }}} getStopSequence */

/* {{{ proto boolean setStartSequence(bool flag)
   */
PHP_METHOD(Mustache, setEscapeByDefault)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;
  
  long flag;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &_this_zval, Mustache_ce_ptr, &flag) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);
  
  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  payload->mustache->setEscapeByDefault((bool) flag != 0);
  
  RETURN_TRUE;
}
/* }}} setEscapeByDefault */

/* {{{ proto boolean setStartSequence(string str)
   */
PHP_METHOD(Mustache, setStartSequence)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;
  
  char * str;
  long str_len;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &_this_zval, Mustache_ce_ptr, &str, &str_len) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  payload->mustache->setStartSequence(str);
  
  RETURN_TRUE;
}
/* }}} setStartSequence */

/* {{{ proto boolean setStopSequence(string str)
   */
PHP_METHOD(Mustache, setStopSequence)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;
  
  char * str;
  long str_len;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &_this_zval, Mustache_ce_ptr, &str, &str_len) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  payload->mustache->setStopSequence(str);
  
  RETURN_TRUE;
}
/* }}} setStartSequence */

/* {{{ proto MustacheTemplate compile(string template)
   */
PHP_METHOD(Mustache, compile)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;
  
  char * template_str;
  long template_len;
  std::string templateStr;
  
  mustache::Node * root;
  
  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &_this_zval, Mustache_ce_ptr, &template_str, &template_len) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  templateStr.assign(template_str);
  
  // Tokenize template
  try {
    
    // Get MustacheTemplate class entry
    std::string className("MustacheTemplate");
    zend_class_entry * MustacheTemplate_ce_ptr;
    php_obj_MustacheTemplate * intern;
    
    MustacheTemplate_ce_ptr = mustache_get_class_entry((char *)className.c_str(), className.length());
    
    if( MustacheTemplate_ce_ptr == NULL ) {
      php_error_docref(NULL TSRMLS_CC, E_WARNING, "Class %s does not exist%s", className.c_str());
      RETURN_FALSE;
      return;
    }
    
    // Initialize new object
    object_init_ex(return_value, MustacheTemplate_ce_ptr);
    intern = (php_obj_MustacheTemplate *) zend_objects_get_address(return_value TSRMLS_CC);
    
    // Get object's internal node pointer
    if( intern->node == NULL ) {
      intern->node = root = new mustache::Node;
    } else {
      root = intern->node;
    }
    
    // Tokenize
    payload->mustache->tokenize(&templateStr, root);
    
    // Ref - not sure if this is required
//    Z_SET_REFCOUNT_P(return_value, 1);
//    Z_SET_ISREF_P(return_value);
    
  } catch( mustache::Exception& e ) {
    
    php_error(E_WARNING, (char *) e.what());
    RETURN_FALSE;
    
  }
}
/* }}} compile */

/* {{{ proto array tokenize(string template)
   */
PHP_METHOD(Mustache, tokenize)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;
  
  char * template_str;
  long template_len;
  std::string templateStr;
  
  mustache::Node root;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &_this_zval, Mustache_ce_ptr, &template_str, &template_len) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  templateStr.assign(template_str);
  
  // Tokenize template
  try {
    
    payload->mustache->tokenize(&templateStr, &root);
    
    // Convert to PHP array
    mustache_node_to_zval(&root, return_value);
    
  } catch( mustache::Exception& e ) {
    
    php_error(E_WARNING, (char *) e.what());
    RETURN_FALSE;
    
  }
}
/* }}} tokenize */

/* {{{ proto string render(string template, array data, array partials)
   */
PHP_METHOD(Mustache, render)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;
  
  char * template_str;
  long template_len;
  
  zval * data = NULL;
  HashTable * data_hash = NULL;
  
  zval * partials = NULL;
  
  std::string templateStr;
  mustache::Node templateNode;
  mustache::Node::Partials templatePartials;
  mustache::Data templateData;
  std::string output;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Osa/|a/", &_this_zval, Mustache_ce_ptr, &template_str, &template_len, &data, &partials) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  try {
    // Prepare template string
    templateStr.assign(template_str);
    
    // Prepare template data
    mustache_data_from_zval(&templateData, data);
    
    // Tokenize template
    payload->mustache->tokenize(&templateStr, &templateNode);
    
    // Tokenize partials
    mustache_partials_from_zval(payload->mustache, &templatePartials, partials);
    
    // Render template
    payload->mustache->render(&templateNode, &templateData, &templatePartials, &output);
    
    // Output
    RETURN_STRING(output.c_str(), 1); // Yes reallocate
    
  } catch( mustache::Exception& e ) {
    
    php_error(E_WARNING, (char *) e.what());
    RETURN_FALSE;
    
  }
}
/* }}} render */

/* {{{ proto array debugDataStructure(array data)
   */
PHP_METHOD(Mustache, debugDataStructure)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;
  
  zval * data = NULL;
  zval * datacpy = NULL;
  
  mustache::Data templateData;
  
  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oa/", &_this_zval, Mustache_ce_ptr, &data) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  // Prepare template data
  mustache_data_from_zval(&templateData, data);
  
  // Reverse template data
  datacpy = mustache_data_to_zval(&templateData);
  
  *return_value = *datacpy;
  zval_copy_ctor(return_value);
}
/* }}} debugDataStructure */
