
#include "php_mustache.hpp"
#include "php_mustache_methods.hpp"

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

/* {{{ proto array tokenize(string template)
   */
PHP_METHOD(Mustache, tokenize)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;
  
  char * template_str;
  long template_len;
  string templateStr;
  
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

/* {{{ proto string render(string template, array data)
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
  
  string templateStr;
  mustache::Node templateNode;
  mustache::Node::Partials templatePartials;
  mustache::Data templateData;
  string output;

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




void mustache_node_to_zval(mustache::Node * node, zval * current)
{
  array_init(current);
  
  // Basic data
  add_assoc_long(current, "type", node->type);
  add_assoc_long(current, "flags", node->flags);
  if( NULL != node->data && node->data->length() > 0 ) {
    add_assoc_stringl(current, "data", (char *) node->data->c_str(), node->data->length(), 1);
  }
  
  // Children
  if( node->children.size() > 0 ) {
    zval * children;
    ALLOC_INIT_ZVAL(children);
    array_init(children);
    
    mustache::Node::Children::iterator it;
    for ( it = node->children.begin() ; it != node->children.end(); it++ ) {
      zval * child;
      ALLOC_INIT_ZVAL(child);
      mustache_node_to_zval(*it, child);
      add_next_index_zval(children, child);
    }
    
    add_assoc_zval(current, "children", children);
  }
}

void mustache_data_from_zval(mustache::Data * node, zval * current)
{
  HashTable * data_hash = NULL;
  HashPosition data_pointer = NULL;
  zval **data_entry = NULL;
  long data_count;
  
  int key_type;
  char * key_str;
  uint key_len;
  ulong key_nindex;
  string * ckey;
  
  int ArrayPos = 0;
  mustache::Data * child = NULL;
  
  switch( Z_TYPE_P(current) ) {
      case IS_NULL:
      case IS_LONG:
      case IS_BOOL:
      case IS_DOUBLE:
      case IS_STRING:
        convert_to_string(current);
        node->type = mustache::Data::TypeString;
        node->val = new string(Z_STRVAL_P(current));
        break;
      case IS_ARRAY: // START IS_ARRAY -----------------------------------------
        node->type = mustache::Data::TypeNone;
        
        data_hash = HASH_OF(current);
        data_count = zend_hash_num_elements(data_hash);
        zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
        while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
          // Get current key
          key_type = zend_hash_get_current_key_ex(data_hash, &key_str, &key_len, &key_nindex, true, &data_pointer);
          // Check key type
          if( key_type == HASH_KEY_IS_LONG ) {
            if( node->type == mustache::Data::TypeNone ) {
              node->init(mustache::Data::TypeArray, data_count);
              child = node->array;
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
            mustache_data_from_zval(child, *data_entry);
            child++;
          } else if( node->type == mustache::Data::TypeMap ) {
            child = new mustache::Data();
            mustache_data_from_zval(child, *data_entry);
            ckey = new string(key_str);
            node->data.insert(pair<string,mustache::Data*>(*ckey,child));
          } else {
            // Whoops
          }
          zend_hash_move_forward_ex(data_hash, &data_pointer);
        }
  
        break; // END IS_ARRAY -------------------------------------------------
    case IS_OBJECT:
      // @todo
    default:
      php_error(E_WARNING, "Invalid data type: %d", Z_TYPE_P(current));
      break;
  }
}

zval * mustache_data_to_zval(mustache::Data * node)
{
  mustache::Data::List::iterator l_it;
  mustache::Data::Map::iterator m_it;
  mustache::Data::Array childNode;
  int pos;
  zval * current;
  zval * child;
  
  ALLOC_INIT_ZVAL(current);
  
  switch( node->type ) {
    case mustache::Data::TypeString:
      Z_TYPE_P(current) = IS_STRING;
      Z_STRVAL_P(current) = (char *) estrdup(node->val->c_str());
      Z_STRLEN_P(current) = node->val->length();
      break;
    case mustache::Data::TypeArray:
      array_init(current);
      childNode = node->array;
      for( pos = 0; pos < node->length; pos++, childNode++ ) {
        child = mustache_data_to_zval(childNode);
        add_next_index_zval(current, child);
      }
      break;
    case mustache::Data::TypeList:
      array_init(current);
      for ( l_it = node->children.begin() ; l_it != node->children.end(); l_it++ ) {
        child = mustache_data_to_zval(*l_it);
        add_next_index_zval(current, child);
      }
      break;
    case mustache::Data::TypeMap:
      array_init(current);
      for ( m_it = node->data.begin() ; m_it != node->data.end(); m_it++ ) {
        child = mustache_data_to_zval((*m_it).second);
        add_assoc_zval(current, (*m_it).first.c_str(), child);
      }
      break;
    default:
      Z_TYPE_P(current) = IS_NULL;
      php_error(E_WARNING, "Invalid data type");
      break;
  }
  
  return current;
}

void mustache_partials_from_zval(mustache::Mustache * mustache, 
        mustache::Node::Partials * partials, zval * current)
{
  ;
}