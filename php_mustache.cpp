
#include "php_mustache.hpp"

#include "mustache_mustache.hpp"
#include "mustache_data.hpp"
#include "mustache_template.hpp"

// Module Entry ----------------------------------------------------------------

zend_module_entry mustache_module_entry = {
  STANDARD_MODULE_HEADER,
  PHP_MUSTACHE_NAME,        // Define PHP extension name  
  NULL,        /* Functions */  
  PHP_MINIT(mustache),        /* MINIT */  
  NULL,        /* MSHUTDOWN */  
  NULL,        /* RINIT */  
  NULL,        /* RSHUTDOWN */  
  PHP_MINFO(mustache),        /* MINFO */  
  PHP_MUSTACHE_VERSION,
  STANDARD_MODULE_PROPERTIES  
};  

#ifdef COMPILE_DL_MUSTACHE 
extern "C" {
  ZEND_GET_MODULE(mustache)      // Common for all PHP extensions which are build as shared modules  
}
#endif

// MINIT -----------------------------------------------------------------------
  
PHP_MINIT_FUNCTION(mustache)
{
  PHP_MINIT(mustache_mustache)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_data)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_template)(INIT_FUNC_ARGS_PASSTHRU);
  
  return SUCCESS;
}

// MINFO -----------------------------------------------------------------------
  
PHP_MINFO_FUNCTION(mustache)
{
  php_info_print_table_start();
  php_info_print_table_row(2, "Version", PHP_MUSTACHE_VERSION);
  php_info_print_table_row(2, "Released", PHP_MUSTACHE_RELEASE);
  php_info_print_table_row(2, "Revision", PHP_MUSTACHE_BUILD);
  php_info_print_table_row(2, "Authors", PHP_MUSTACHE_AUTHORS);
#if HAVE_TCMALLOC
  php_info_print_table_row(2, "tcmalloc support", "enabled");
#else
  php_info_print_table_row(2, "tcmalloc support", "disabled");
#endif
#if HAVE_TR1
  php_info_print_table_row(2, "tr1 support", "enabled");
#else
  php_info_print_table_row(2, "tr1 support", "disabled");
#endif
#if HAVE_LIBBOOST
  php_info_print_table_row(2, "libboost support", "enabled");
#else
  php_info_print_table_row(2, "libboost support", "disabled");
#endif
  php_info_print_table_end();
}

// Utils -----------------------------------------------------------------------

void mustache_node_to_zval(mustache::Node * node, zval * current TSRMLS_DC)
{
  zval * children = NULL;
  
  array_init(current);
  
  // Basic data
  add_assoc_long(current, "type", node->type);
  add_assoc_long(current, "flags", node->flags);
  if( NULL != node->data && node->data->length() > 0 ) {
    add_assoc_stringl(current, "data", (char *) node->data->c_str(), node->data->length(), 1);
  }
  
  // Children
  if( node->children.size() > 0 ) {
    ALLOC_INIT_ZVAL(children);
    array_init(children);
    
    mustache::Node::Children::iterator it;
    for ( it = node->children.begin() ; it != node->children.end(); it++ ) {
      zval * child;
      ALLOC_INIT_ZVAL(child);
      mustache_node_to_zval(*it, child TSRMLS_CC);
      add_next_index_zval(children, child);
    }
    
    add_assoc_zval(current, "children", children);
    children = NULL;
  }
  
  // Partials
  if( node->partials.size() > 0 ) {
    ALLOC_INIT_ZVAL(children);
    array_init(children);
    
    mustache::Node::Partials::iterator it;
    for ( it = node->partials.begin() ; it != node->partials.end(); it++ ) {
      zval * child;
      ALLOC_INIT_ZVAL(child);
      mustache_node_to_zval(&(it->second), child TSRMLS_CC);
      add_assoc_zval(children, it->first.c_str(), child);
    }
    
    add_assoc_zval(current, "partials", children);
    children = NULL;
  }
}

void mustache_data_from_zval(mustache::Data * node, zval * current TSRMLS_DC)
{
  HashTable * data_hash = NULL;
  HashPosition data_pointer = NULL;
  zval **data_entry = NULL;
  long data_count = 0;
  
  int key_type = 0;
  char * key_str = NULL;
  uint key_len = 0;
  ulong key_nindex = 0;
  std::string ckey;
  
  int ArrayPos = 0;
  mustache::Data * child = NULL;
  zend_class_entry * ce = NULL;
  
  switch( Z_TYPE_P(current) ) {
      case IS_NULL:
      case IS_LONG:
      case IS_BOOL:
      case IS_DOUBLE:
      case IS_STRING:
        convert_to_string(current);
        node->type = mustache::Data::TypeString;
        node->val = new std::string(Z_STRVAL_P(current)/*, (size_t) Z_STRLEN_P(current)*/);
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
            mustache_data_from_zval(child, *data_entry TSRMLS_CC);
            child++;
          } else if( node->type == mustache::Data::TypeMap ) {
            child = new mustache::Data;
            mustache_data_from_zval(child, *data_entry TSRMLS_CC);
            ckey.assign(key_str);
            node->data.insert(std::pair<std::string,mustache::Data*>(ckey,child));
          } else {
            // Whoops
          }
          zend_hash_move_forward_ex(data_hash, &data_pointer);
        }
  
        break; // END IS_ARRAY -------------------------------------------------
    case IS_OBJECT:
      ce = Z_OBJCE_P(current);
      if( ce != NULL ) {
        std::string className("MustacheData");
        zend_class_entry * mtce = mustache_get_class_entry((char *)className.c_str(), className.length() TSRMLS_CC);
        if( mtce == NULL ) {
          php_error(E_WARNING, "Invalid object type (2)");
        } else if( mtce != ce ) {
          php_error(E_WARNING, "Invalid object type (3)");
        } else {
          // @todo
          php_error(E_WARNING, "MustacheData not implemented here");
        }
      } else {
        php_error(E_WARNING, "Invalid object type (1)");
      }
      break;
    default:
      php_error(E_WARNING, "Invalid data type: %d", Z_TYPE_P(current));
      break;
  }
}

zval * mustache_data_to_zval(mustache::Data * node TSRMLS_DC)
{
  mustache::Data::List::iterator l_it;
  mustache::Data::Map::iterator m_it;
  mustache::Data::Array childNode;
  int pos = 0;
  zval * current = NULL;
  zval * child = NULL;
  
  ALLOC_INIT_ZVAL(current);
  
  switch( node->type ) {
    case mustache::Data::TypeNone:
      Z_TYPE_P(current) = IS_NULL;
      break;
    case mustache::Data::TypeString:
      Z_TYPE_P(current) = IS_STRING;
      Z_STRVAL_P(current) = (char *) estrdup(node->val->c_str());
      Z_STRLEN_P(current) = node->val->length();
      break;
    case mustache::Data::TypeArray:
      array_init(current);
      childNode = node->array;
      for( pos = 0; pos < node->length; pos++, childNode++ ) {
        child = mustache_data_to_zval(childNode TSRMLS_CC);
        add_next_index_zval(current, child);
      }
      break;
    case mustache::Data::TypeList:
      array_init(current);
      for ( l_it = node->children.begin() ; l_it != node->children.end(); l_it++ ) {
        child = mustache_data_to_zval(*l_it TSRMLS_CC);
        add_next_index_zval(current, child);
      }
      break;
    case mustache::Data::TypeMap:
      array_init(current);
      for ( m_it = node->data.begin() ; m_it != node->data.end(); m_it++ ) {
        child = mustache_data_to_zval((*m_it).second TSRMLS_CC);
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

zend_class_entry * mustache_get_class_entry(char * name, int len TSRMLS_DC)
{
  zend_class_entry ** ce = NULL;
  int found = 0;
  char * lc_name = NULL;
  
  ALLOCA_FLAG(use_heap)

  lc_name = (char *) do_alloca(len + 1, use_heap);
  zend_str_tolower_copy(lc_name, name, len);

  found = zend_hash_find(EG(class_table), lc_name, len + 1, (void **) &ce);
  free_alloca(lc_name, use_heap);

  if( found != SUCCESS ) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Class %s does not exist", name);
    return NULL;
  } else {
    return *ce;
  }
}

void mustache_exception_handler()
{
#if PHP_MUSTACHE_THROW_EXCEPTIONS
  throw;
#else
  try {
    throw;
  } catch( mustache::Exception& e ) {
    // @todo change this to an exception
    php_error_docref(NULL TSRMLS_CC, E_WARNING, e.what());
  } catch( InvalidParameterException& e ) {
    // @todo change this to an exception
    php_error_docref(NULL TSRMLS_CC, E_WARNING, e.what());
  } catch( PhpInvalidParameterException& e ) {
    // The error message should be handled by PHP
  } catch( std::bad_alloc& e ) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "Memory allocation failed.");
  } catch( std::runtime_error& e ) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, e.what());
  } catch(...) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "An unknown error has occurred.");
  }
#endif
}

bool mustache_parse_template_param(zval * tmpl, mustache::Mustache * mustache,
        mustache::Node ** node TSRMLS_DC)
{
  // Prepare template string
  if( Z_TYPE_P(tmpl) == IS_STRING ) {
    // Tokenize template
    char * tmpstr = Z_STRVAL_P(tmpl);
    *node = new mustache::Node();
    try {
      std::string templateStr(tmpstr/*, (size_t) Z_STRLEN_P(tmpl)*/);
      mustache->tokenize(&templateStr, *node);
    } catch(...) {
      delete *node; // Prevent leaks
      throw;
    }
    return true;

  } else if( Z_TYPE_P(tmpl) == IS_OBJECT ) {
    // Use compiled template
    std::string mtClassName("MustacheTemplate");
    zend_class_entry * tmp_ce = Z_OBJCE_P(tmpl);
    zend_class_entry * mtce = mustache_get_class_entry((char *)mtClassName.c_str(), mtClassName.length() TSRMLS_CC);
    if( tmp_ce == NULL || mtce == NULL || tmp_ce != mtce ) {
      php_error(E_WARNING, "Object not an instance of MustacheTemplate");
      return false;
    } else {
      php_obj_MustacheTemplate * mtPayload = 
              (php_obj_MustacheTemplate *) zend_object_store_get_object(tmpl TSRMLS_CC);
      if( mtPayload->node == NULL ) {
        if( mtPayload->tmpl == NULL ) {
          php_error(E_WARNING, "Empty MustacheTemplate");
          return false;
        } else {
          // This won't leak because the payload handles destruction
          mtPayload->node = new mustache::Node();
          mustache->tokenize(mtPayload->tmpl, mtPayload->node);
        }
      }
      *node = mtPayload->node;
      return true;
    }
  } else {
    php_error(E_WARNING, "Invalid argument");
    return false;
  }
}

bool mustache_parse_data_param(zval * data, mustache::Mustache * mustache,
        mustache::Data ** node TSRMLS_DC)
{
  zend_class_entry * tmp_ce = NULL;
  std::string mdClassName("MustacheData");
  zend_class_entry * mdce = NULL;
  php_obj_MustacheData * mdPayload = NULL;
  
  if( Z_TYPE_P(data) == IS_OBJECT ) {
    tmp_ce = Z_OBJCE_P(data);
    mdce = mustache_get_class_entry((char *)mdClassName.c_str(), mdClassName.length() TSRMLS_CC);
    if( tmp_ce == NULL || mdce == NULL || tmp_ce != mdce ) {
      php_error(E_WARNING, "Object not an instance of MustacheData");
      return false;
    } else {
      mdPayload = (php_obj_MustacheData *) zend_object_store_get_object(data TSRMLS_CC);
      *node = mdPayload->data;
      return true;
    }
  } else {
    mustache_data_from_zval(*node, data TSRMLS_CC);
    return true;
  }
}

bool mustache_parse_partials_param(zval * array, mustache::Mustache * mustache,
        mustache::Node::Partials * partials TSRMLS_DC)
{
  HashTable * data_hash = NULL;
  HashPosition data_pointer = NULL;
  zval **data_entry = NULL;
  long data_count = 0;
  
  int key_type = 0;
  char * key_str = NULL;
  uint key_len = 0;
  ulong key_nindex = 0;
  std::string ckey;
  
  std::string tmpl;
  mustache::Node node;
  mustache::Node * nodePtr = NULL;
  
  std::string mtClassName("MustacheTemplate");
  zend_class_entry * tmp_ce = NULL;
  zend_class_entry * mtce = NULL;
  php_obj_MustacheTemplate * mtPayload = NULL;
  
  // Ignore if not an array
  if( array == NULL || Z_TYPE_P(array) != IS_ARRAY ) {
    return false;
  }
  
  // Get MustacheTemplate class entry
  mtce = mustache_get_class_entry((char *)mtClassName.c_str(), mtClassName.length() TSRMLS_CC);
  
  // Iterate over input data
  data_hash = HASH_OF(array);
  data_count = zend_hash_num_elements(data_hash);
  zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
  while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
    // Get current key
    key_type = zend_hash_get_current_key_ex(data_hash, &key_str, &key_len, &key_nindex, true, &data_pointer);
    // Check key type
    if( key_type != HASH_KEY_IS_STRING ) {
      // Non-string key
      php_error(E_WARNING, "Partial array contains a non-string key");
    } else if( Z_TYPE_PP(data_entry) == IS_STRING ) {
      // String key, string value
      ckey.assign(key_str);
      tmpl.assign(Z_STRVAL_PP(data_entry));
      partials->insert(make_pair(ckey, node));
      mustache->tokenize(&tmpl, &(*partials)[ckey]);
    } else if( Z_TYPE_PP(data_entry) == IS_OBJECT ) {
      // String key, object value
      tmp_ce = Z_OBJCE_PP(data_entry);
      if( tmp_ce == NULL || mtce == NULL || tmp_ce != mtce ) {
        php_error(E_WARNING, "Object not an instance of MustacheTemplate");
      } else {
        ckey.assign(key_str);
        mtPayload = (php_obj_MustacheTemplate *) zend_object_store_get_object(*data_entry TSRMLS_CC);
        partials->insert(make_pair(ckey, node));
        
        // This is kind of hack-ish
        nodePtr = &(*partials)[ckey];
        nodePtr->type = mustache::Node::TypeContainer;
        nodePtr->child = mtPayload->node;
      }
    } else {
      php_error(E_WARNING, "Partial array contains an invalid value");
    }
    zend_hash_move_forward_ex(data_hash, &data_pointer);
  }
}