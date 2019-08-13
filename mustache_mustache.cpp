
#include "php_mustache.h"
#include "mustache_private.hpp"
#include "mustache_ast.hpp"
#include "mustache_data.hpp"
#include "mustache_exceptions.hpp"
#include "mustache_template.hpp"
#include "mustache_mustache.hpp"

/* {{{ ZE2 OO definitions */
zend_class_entry * Mustache_ce_ptr;
static zend_object_handlers Mustache_obj_handlers;
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(Mustache____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__getEscapeByDefault_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__getStartSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__getStopSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__setEscapeByDefault_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, escapeByDefault)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__setStartSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, startSequence)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__setStopSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, stopSequence)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__parse_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__render_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 3)
	ZEND_ARG_INFO(0, str)
        ZEND_ARG_INFO(0, vars)
        ZEND_ARG_INFO(0, partials)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__tokenize_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Mustache__debugDataStructure_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
        ZEND_ARG_INFO(0, vars)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ Mustache_methods */
static zend_function_entry Mustache_methods[] = {
  PHP_ME(Mustache, __construct, Mustache____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(Mustache, getEscapeByDefault, Mustache__getEscapeByDefault_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, getStartSequence, Mustache__getStartSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, getStopSequence, Mustache__getStopSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setEscapeByDefault, Mustache__setEscapeByDefault_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setStartSequence, Mustache__setStartSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, setStopSequence, Mustache__setStopSequence_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, parse, Mustache__parse_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, render, Mustache__render_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, tokenize, Mustache__tokenize_args, ZEND_ACC_PUBLIC)
  PHP_ME(Mustache, debugDataStructure, Mustache__debugDataStructure_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};
/* }}} */

/* {{{ php_mustache_mustache_object_fetch_object */
static inline struct php_obj_Mustache * php_mustache_mustache_fetch_object(zend_object * obj)
{
  return (struct php_obj_Mustache *) ((char *)(obj) - XtOffsetOf(struct php_obj_Mustache, std));
}

struct php_obj_Mustache * php_mustache_mustache_object_fetch_object(zval * zv)
{
  return php_mustache_mustache_fetch_object(Z_OBJ_P(zv));
}
/* }}} */

/* {{{ Mustache_obj_free */
static void Mustache_obj_free(zend_object * object)
{
  try {
    struct php_obj_Mustache * payload = php_mustache_mustache_fetch_object(object);
    
    if( payload->mustache != NULL ) {
      delete payload->mustache;
    }
    
    zend_object_std_dtor((zend_object *) object);
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} */

/* {{{ Mustache_obj_create */
static zend_object * Mustache_obj_create(zend_class_entry * ce)
{
  struct php_obj_Mustache * intern;
  
  try {
    intern = (struct php_obj_Mustache *) ecalloc(1, sizeof(php_obj_Mustache) + zend_object_properties_size(ce));
    zend_object_std_init(&intern->std, ce);
    intern->std.handlers = &Mustache_obj_handlers;

    intern->mustache = mustache_new_Mustache();
    return &intern->std;
  } catch(...) {
    mustache_exception_handler();
  }
  
  return NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mustache_mustache)
{
  try {
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Mustache", Mustache_methods);
    ce.create_object = Mustache_obj_create;
    Mustache_ce_ptr = zend_register_internal_class(&ce);
    memcpy(&Mustache_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    Mustache_obj_handlers.offset = XtOffsetOf(php_obj_Mustache, std);
    Mustache_obj_handlers.free_obj = Mustache_obj_free;
    Mustache_obj_handlers.clone_obj = NULL;

    return SUCCESS;
  } catch(...) {
    mustache_exception_handler();
    return FAILURE;
  }
}
/* }}} */

/* {{{ mustache_new_Mustache */
mustache::Mustache * mustache_new_Mustache() {
  mustache::Mustache * mustache = new mustache::Mustache();

  // Set ini settings
  if( MUSTACHEG(default_escape_by_default) ) {
    mustache->setEscapeByDefault(true);
  } else {
    mustache->setEscapeByDefault(false);
  }
  if( MUSTACHEG(default_start_sequence) ) {
    mustache->setStartSequence(MUSTACHEG(default_start_sequence), 0);
  }
  if( MUSTACHEG(default_stop_sequence) ) {
    mustache->setStopSequence(MUSTACHEG(default_stop_sequence), 0);
  }

  return mustache;
}
/* }}} */

/* {{{ mustache_parse_data_param */
bool mustache_parse_data_param(zval * data, mustache::Mustache * mustache, mustache::Data ** node)
{
  struct php_obj_MustacheData * mdPayload = NULL;
  
  if( Z_TYPE_P(data) == IS_OBJECT ) {
    if( Z_OBJCE_P(data) == MustacheData_ce_ptr ) {
      mdPayload = php_mustache_data_object_fetch_object(data);
      *node = mdPayload->data;
      return true;
    } else {
      mustache_data_from_zval(*node, data);
      return true;
    }
  } else {
    mustache_data_from_zval(*node, data);
    return true;
  }
}
/* }}} */

/* {{{ mustache_parse_partials_param */
static inline void mustache_parse_partial_param(char * key, zval * data,
        mustache::Mustache * mustache, mustache::Node::Partials * partials)
{
    std::string ckey;
    std::string tmpl;
    mustache::Node node;
    mustache::Node * nodePtr = NULL;
    struct php_obj_MustacheAST * maPayload;

    if( Z_TYPE_P(data) == IS_STRING ) {
      // String key, string value
      ckey.assign(key);
      tmpl.assign(Z_STRVAL_P(data));
      partials->insert(std::make_pair(ckey, node));
      mustache->tokenize(&tmpl, &(*partials)[ckey]);
    } else if( Z_TYPE_P(data) == IS_OBJECT ) {
        // String key, object value
        if( Z_OBJCE_P(data) == MustacheTemplate_ce_ptr ) {
            zval rv;
            zval * value = zend_read_property(Z_OBJCE_P(data), data, "template", sizeof("template")-1, 1, &rv);
            convert_to_string(value);
            std::string tmpstr(Z_STRVAL_P(value));
            ckey.assign(key);
            partials->insert(std::make_pair(ckey, node));
            mustache->tokenize(&tmpstr, &(*partials)[ckey]);
        } else if( Z_OBJCE_P(data) == MustacheAST_ce_ptr ) {
            ckey.assign(key);
            maPayload = php_mustache_ast_object_fetch_object(data);
            partials->insert(std::make_pair(ckey, node));
            nodePtr = &(*partials)[ckey];
            nodePtr->type = mustache::Node::TypeContainer;
            nodePtr->child = maPayload->node;
        } else {
            php_error(E_WARNING, "Object not an instance of MustacheTemplate or MustacheAST");
        }
    } else {
        php_error(E_WARNING, "Partial array contains an invalid value");
    }
}

bool mustache_parse_partials_param(zval * array, mustache::Mustache * mustache,
        mustache::Node::Partials * partials)
{
    HashTable * data_hash = NULL;
    ulong key_nindex = 0;

    // Ignore if not an array
    if( array == NULL || Z_TYPE_P(array) != IS_ARRAY ) {
        return false;
    }

    data_hash = HASH_OF(array);

    zval * data_entry = NULL;
    zend_string * key = NULL;

    ZEND_HASH_FOREACH_KEY_VAL(data_hash, key_nindex, key, data_entry) {
        (void)key_nindex;
        if( !key ) {
            php_error(E_WARNING, "Partial array contains a non-string key");
        } else {
            mustache_parse_partial_param(key->val, data_entry, mustache, partials);
        }
    } ZEND_HASH_FOREACH_END();

  return true;
}
/* }}} */

/* {{{ mustache_parse_template_param */
bool mustache_parse_template_param(zval * tmpl, mustache::Mustache * mustache,
        mustache::Node ** node)
{
  // Prepare template string
  if( Z_TYPE_P(tmpl) == IS_STRING ) {
    // Tokenize template
    char * tmpstr = Z_STRVAL_P(tmpl);
    std::string templateStr(tmpstr/*, (size_t) Z_STRLEN_P(tmpl)*/);
    mustache->tokenize(&templateStr, *node);
    return true;

  } else if( Z_TYPE_P(tmpl) == IS_OBJECT ) {
    // Use compiled template
    if( Z_OBJCE_P(tmpl) == MustacheTemplate_ce_ptr ) {
        zval rv;
        zval * value = zend_read_property(Z_OBJCE_P(tmpl), tmpl, "template", sizeof("template")-1, 1, &rv);
        convert_to_string(value);
        std::string tmpstr(Z_STRVAL_P(value));
      
      if( !tmpstr.length() ) {
        php_error(E_WARNING, "Empty MustacheTemplate");
        return false;
      } else {
        mustache->tokenize(&tmpstr, *node);
      }
      return true;
    } else if( Z_OBJCE_P(tmpl) == MustacheAST_ce_ptr ) {
      struct php_obj_MustacheAST * maPayload = php_mustache_ast_object_fetch_object(tmpl);
      if( maPayload->node == NULL ) {
        php_error(E_WARNING, "Empty MustacheAST");
        return false;
      }
      *node = maPayload->node;
      return true;
    } else {
      php_error(E_WARNING, "Object not an instance of MustacheTemplate or MustacheAST");
      return false;
    }
  } else {
    php_error(E_WARNING, "Invalid argument");
    return false;
  }
}
/* }}} */

/* {{{ proto void Mustache::__construct() */
PHP_METHOD(Mustache, __construct)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::__construct */

/* {{{ proto boolean Mustache::getEscapeByDefault() */
PHP_METHOD(Mustache, getEscapeByDefault)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    
    // Main
    if( payload->mustache->getEscapeByDefault() ) {
      RETURN_TRUE;
    } else {
      RETURN_FALSE;
    }
    
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::getEscapeByDefault */

/* {{{ proto string Mustache::getStartSequence() */
PHP_METHOD(Mustache, getStartSequence)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    
    // Main
    const std::string & str = payload->mustache->getStartSequence();
    RETVAL_STRINGL(str.c_str(), str.length());
    
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::getStartSequence */

/* {{{ proto string Mustache::getStopSequence() */
PHP_METHOD(Mustache, getStopSequence)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "O",
            &_this_zval, Mustache_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    
    // Main
    const std::string & str = payload->mustache->getStopSequence();
    RETVAL_STRINGL(str.c_str(), str.length());
    
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::getStopSequence */

/* {{{ proto boolean Mustache::setStartSequence(bool flag) */
PHP_METHOD(Mustache, setEscapeByDefault)
{
  try {
    // Custom parameters
    long flag = 0;
  
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Ol",
            &_this_zval, Mustache_ce_ptr, &flag) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
  
    // Main
    payload->mustache->setEscapeByDefault((bool) flag != 0);
    RETURN_TRUE;
    
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::setEscapeByDefault */

/* {{{ proto boolean Mustache::setStartSequence(string str) */
PHP_METHOD(Mustache, setStartSequence)
{
  try {
    // Custom parameters
    char * str = NULL;
    long str_len = 0;
    
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Os",
            &_this_zval, Mustache_ce_ptr, &str, &str_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    
    // Main
    payload->mustache->setStartSequence(str/*, str_len*/);
    RETURN_TRUE;
    
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::setStartSequence */

/* {{{ proto boolean Mustache::setStopSequence(string str) */
PHP_METHOD(Mustache, setStopSequence)
{
  try {
    // Custom parameters
    char * str = NULL;
    long str_len = 0;
    
    // Check parameters
    zval * _this_zval;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Os",
            &_this_zval, Mustache_ce_ptr, &str, &str_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    
    // Main
    payload->mustache->setStopSequence(str/*, str_len*/);
    RETURN_TRUE;
    
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::setStartSequence */

/* {{{ proto MustacheAST Mustache::parse(string template) */
PHP_METHOD(Mustache, parse)
{
  try {
    // Custom parameters
    zval * tmpl = NULL;
  
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Oz",
            &_this_zval, Mustache_ce_ptr, &tmpl) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    
    // Check template parameter
    mustache::Node * templateNodePtr = new mustache::Node();
    if( !mustache_parse_template_param(tmpl, payload->mustache, &templateNodePtr) ) {
      delete templateNodePtr;
      RETURN_FALSE;
      return;
    }
    
    // Handle return value
    if( Z_TYPE_P(tmpl) == IS_STRING ) {
      if( MustacheAST_ce_ptr == NULL ) {
        delete templateNodePtr;
        php_error_docref(NULL, E_WARNING, "Class MustacheAST does not exist");
        RETURN_FALSE;
        return;
      }
      
      // Initialize new object
      object_init_ex(return_value, MustacheAST_ce_ptr);
      struct php_obj_MustacheAST * intern = php_mustache_ast_object_fetch_object(return_value);
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
    mustache_exception_handler();
  }
}
/* }}} Mustache::parse */

/* {{{ proto string Mustache::render(mixed template, array data, array partials) */
PHP_METHOD(Mustache, render)
{
  try {
    // Custom parameters
    zval * tmpl = NULL;
    zval * data = NULL;
    zval * partials = NULL;
    
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Ozz|a/",
            &_this_zval, Mustache_ce_ptr, &tmpl, &data, &partials) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    
    // Prepare template tree
    mustache::Node templateNode;
    mustache::Node * templateNodePtr = &templateNode;
    if( !mustache_parse_template_param(tmpl, payload->mustache, &templateNodePtr) ) {
      RETURN_FALSE;
      return;
    }
    
    // Prepare template data
    mustache::Data templateData;
    mustache::Data * templateDataPtr = &templateData;
    if( !mustache_parse_data_param(data, payload->mustache, &templateDataPtr) ) {
      RETURN_FALSE;
      return;
    }
    
    // Tokenize partials
    mustache::Node::Partials templatePartials;
    mustache_parse_partials_param(partials, payload->mustache, &templatePartials);
    
    // Reserve length of template
    std::string output;
    if( Z_TYPE_P(tmpl) == IS_STRING ) {
      output.reserve(Z_STRLEN_P(tmpl));
    }
    
    // Render template
    payload->mustache->render(templateNodePtr, templateDataPtr, &templatePartials, &output);
    
    // Output
    RETVAL_STRINGL(output.c_str(), output.length());
    
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::render */

/* {{{ proto array Mustache::tokenize(string template) */
PHP_METHOD(Mustache, tokenize)
{
  try {
    // Custom parameters
    char * template_str = NULL;
    long template_len = 0;
  
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Os",
            &_this_zval, Mustache_ce_ptr, &template_str, &template_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    struct php_obj_Mustache * payload = php_mustache_mustache_object_fetch_object(_this_zval);
    
    // Assign template to string
    std::string templateStr(template_str/*, template_len*/);
    
    // Tokenize template
    mustache::Node root;
    payload->mustache->tokenize(&templateStr, &root);
    
    // Convert to PHP array
    mustache_node_to_zval(&root, return_value);
    
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::tokenize */

/* {{{ proto array Mustache::debugDataStructure(array data) */
PHP_METHOD(Mustache, debugDataStructure)
{
  try {
    // Custom parameters
    zval * data = NULL;
    
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), (char *) "Oz",
            &_this_zval, Mustache_ce_ptr, &data) == FAILURE) {
      throw PhpInvalidParameterException();
    }

    // Class parameters
    _this_zval = getThis();
    
    // Prepare template data
    mustache::Data templateData;
    mustache_data_from_zval(&templateData, data);

    // Reverse template data
    mustache_data_to_zval(&templateData, return_value);
  
  } catch(...) {
    mustache_exception_handler();
  }
}
/* }}} Mustache::debugDataStructure */
