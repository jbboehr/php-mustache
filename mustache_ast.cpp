
#include "mustache_private.hpp"

/* {{{ ZE2 OO definitions */
zend_class_entry * MustacheAST_ce_ptr;
static zend_object_handlers MustacheAST_obj_handlers;
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(MustacheAST____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheAST____sleep_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheAST__toArray_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheAST____toString_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(MustacheAST____wakeup_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ MustacheAST_methods */
static zend_function_entry MustacheAST_methods[] = {
  PHP_ME(MustacheAST, __construct, MustacheAST____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheAST, __sleep, MustacheAST____sleep_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, toArray, MustacheAST__toArray_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, __toString, MustacheAST____toString_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheAST, __wakeup, MustacheAST____wakeup_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};
/* }}} */

/* {{{ mustache_node_from_binary_string */
void mustache_node_from_binary_string(mustache::Node ** node, char * str, int len)
{
  std::vector<uint8_t> uint_str;
  uint_str.resize(len);
  int i = 0;
  for( i = 0; i < len; i++ ) {
    uint_str[i] = str[i];
  }
  
  size_t vpos = 0;
  *node = mustache::Node::unserialize(uint_str, 0, &vpos);
}
/* }}} */

/* {{{ mustache_node_to_binary_string */
void mustache_node_to_binary_string(mustache::Node * node, char ** estr, int * elen)
{
  std::vector<uint8_t> * serialPtr = node->serialize();
  std::vector<uint8_t> & serial = *serialPtr;
  int serialLen = serial.size();
  
  char * str = (char *) emalloc(sizeof(char *) * (serialLen + 1));
  for( int i = 0 ; i < serialLen; i++ ) {
    str[i] = (char) serial[i];
  }
  str[serialLen] = '\0';
  delete serialPtr;
  
  *elen = serialLen;
  *estr = str;
}
/* }}} */

/* {{{ mustache_node_to_zval */
void mustache_node_to_zval(mustache::Node * node, zval * current TSRMLS_DC)
{
  _DECLARE_ZVAL(children);
  _DECLARE_ZVAL(child);
  
  array_init(current);
  
  // Basic data
  add_assoc_long(current, "type", node->type);
  add_assoc_long(current, "flags", node->flags);
  if( NULL != node->data && node->data->length() > 0 ) {
    _add_assoc_stringl_ex(current, _STRS("data"), (char *) node->data->c_str(), node->data->length());
  }
  
  // Children
  if( node->children.size() > 0 ) {
    _ALLOC_INIT_ZVAL(children);
    array_init(children);
    
    mustache::Node::Children::iterator it;
    for ( it = node->children.begin() ; it != node->children.end(); it++ ) {
      _ALLOC_INIT_ZVAL(child);
      mustache_node_to_zval(*it, child TSRMLS_CC);
      add_next_index_zval(children, child);
    }
    
    add_assoc_zval(current, "children", children);
    children = NULL;
  }
  
  // Partials
  if( node->partials.size() > 0 ) {
    _ALLOC_INIT_ZVAL(children);
    array_init(children);
    
    mustache::Node::Partials::iterator it;
    for ( it = node->partials.begin() ; it != node->partials.end(); it++ ) {
      _ALLOC_INIT_ZVAL(child);
      mustache_node_to_zval(&(it->second), child TSRMLS_CC);
      add_assoc_zval(children, it->first.c_str(), child);
    }
    
    add_assoc_zval(current, "partials", children);
    children = NULL;
  }
}
/* }}} */

/* {{{ php_mustache_ast_object_fetch_object */
#if PHP_MAJOR_VERSION < 7
struct php_obj_MustacheAST * php_mustache_ast_object_fetch_object(zval * zv TSRMLS_DC) {
  return (struct php_obj_MustacheAST *) zend_object_store_get_object(zv TSRMLS_CC);
}
#else
static inline struct php_obj_MustacheAST * php_mustache_ast_fetch_object(zend_object * obj TSRMLS_DC)
{
  return (struct php_obj_MustacheAST *)((char*)(obj) - XtOffsetOf(struct php_obj_MustacheAST, std));
}

struct php_obj_MustacheAST * php_mustache_ast_object_fetch_object(zval * zv TSRMLS_DC)
{
  return php_mustache_ast_fetch_object(Z_OBJ_P(zv) TSRMLS_CC);
}
#endif
/* }}} */

/* {{{ MustacheAST_obj_free */
#if PHP_MAJOR_VERSION < 7
static void MustacheAST_obj_free(void *object TSRMLS_DC)
{
  try {
    struct php_obj_MustacheAST * payload = (struct php_obj_MustacheAST *) object;
    
    if( payload->node != NULL ) {
      delete payload->node;
    }
    
    zend_object_std_dtor((zend_object *)object TSRMLS_CC);

    efree(object);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
#else
static void MustacheAST_obj_free(zend_object * object TSRMLS_DC)
{
  try {
    struct php_obj_MustacheAST * payload = php_mustache_ast_fetch_object(object TSRMLS_CC);
    
    if( payload->node != NULL ) {
      delete payload->node;
    }
    
    zend_object_std_dtor((zend_object *)object TSRMLS_CC);
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
#endif
/* }}} */

/* {{{ MustacheAST_obj_create */
#if PHP_MAJOR_VERSION < 7
static zend_object_value MustacheAST_obj_create(zend_class_entry * class_type TSRMLS_DC)
{
  zend_object_value retval;
  
  try {
    struct php_obj_MustacheAST * payload;
    zval * tmp;

    payload = (struct php_obj_MustacheAST *) emalloc(sizeof(struct php_obj_MustacheAST));
    memset(payload, 0, sizeof(struct php_obj_MustacheAST));
    
    zend_object_std_init((zend_object *) payload, class_type TSRMLS_CC);
    
#if PHP_VERSION_ID < 50399
    zend_hash_copy(payload->std.properties, &(class_type->default_properties),
        (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval*));
#else
    object_properties_init(&payload->std, class_type);
#endif
    
    payload->node = NULL; //new mustache::Node();

    retval.handle = zend_objects_store_put(payload, NULL, (zend_objects_free_object_storage_t) MustacheAST_obj_free, NULL TSRMLS_CC);
    retval.handlers = (zend_object_handlers *) &MustacheAST_obj_handlers;
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
  
  return retval;
}
#else
static zend_object * MustacheAST_obj_create(zend_class_entry * ce TSRMLS_DC)
{
  struct php_obj_MustacheAST * intern;
  
  try {
    intern = (struct php_obj_MustacheAST *) ecalloc(1, sizeof(struct php_obj_MustacheAST) + zend_object_properties_size(ce));
    zend_object_std_init(&intern->std, ce TSRMLS_CC);
    intern->std.handlers = &MustacheAST_obj_handlers;
    intern->node = NULL;
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
  
  return &intern->std;
}
#endif
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(mustache_ast)
{
  try {
    zend_class_entry ce;

    memcpy(&MustacheAST_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
#if PHP_MAJOR_VERSION >= 7
    MustacheAST_obj_handlers.offset = XtOffsetOf(struct php_obj_MustacheAST, std);
    MustacheAST_obj_handlers.free_obj = MustacheAST_obj_free;
#endif
    MustacheAST_obj_handlers.clone_obj = NULL;
    
    INIT_CLASS_ENTRY(ce, "MustacheAST", MustacheAST_methods);
    ce.create_object = MustacheAST_obj_create;
    
    MustacheAST_ce_ptr = zend_register_internal_class(&ce TSRMLS_CC);
    MustacheAST_ce_ptr->create_object = MustacheAST_obj_create;
    
    zend_declare_property_null(MustacheAST_ce_ptr, ZEND_STRL("binaryString"), ZEND_ACC_PROTECTED TSRMLS_CC);
    
    return SUCCESS;
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
    return FAILURE;
  }
}
/* }}} */

/* {{{ proto void MustacheAST::__construct(string binaryString) */
PHP_METHOD(MustacheAST, __construct)
{
  try {
    // Custom parameters
    char * str = NULL;
    long str_len = 0;
    
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O|s", 
            &_this_zval, MustacheAST_ce_ptr, &str, &str_len) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval TSRMLS_CC);
    
    // Check payload
    if( payload->node != NULL ) {
      throw InvalidParameterException("MustacheAST is already initialized");
    }
    
    // Unserialize
    mustache_node_from_binary_string(&payload->node, str, str_len);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} MustacheAST::__construct */

/* {{{ proto void MustacheAST::__sleep() */
PHP_METHOD(MustacheAST, __sleep)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O", 
            &_this_zval, MustacheAST_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval TSRMLS_CC);
    
    array_init(return_value);
    
    // Check payload
    if( payload->node != NULL ) {
      // Serialize and store
      char * str = NULL;
      int len = 0;
      mustache_node_to_binary_string(payload->node, &str, &len);
      if( str != NULL ) {
        zend_update_property_stringl(MustacheAST_ce_ptr, _this_zval, 
              ZEND_STRL("binaryString"), str, len TSRMLS_CC);
        _add_next_index_string(return_value, "binaryString");
        efree(str);
      }
    }
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} MustacheAST::__sleep */

/* {{{ proto array MustacheAST::toArray() */
PHP_METHOD(MustacheAST, toArray)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O", 
            &_this_zval, MustacheAST_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval TSRMLS_CC);
    
    // Check payload
    if( payload->node == NULL ) {
      throw InvalidParameterException("MustacheAST was not initialized properly");
    }
    
    // Convert to PHP array
    mustache_node_to_zval(payload->node, return_value TSRMLS_CC);
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} MustacheAST::toArray */

/* {{{ proto string MustacheAST::__toString() */
PHP_METHOD(MustacheAST, __toString)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O", 
            &_this_zval, MustacheAST_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    // Class parameters
    _this_zval = getThis();
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval TSRMLS_CC);
    
    // Check payload
    if( payload->node == NULL ) {
      throw InvalidParameterException("MustacheAST was not initialized properly");
    }
    
    // Convert to PHP binary string
    char * str = NULL;
    int len = 0;
    mustache_node_to_binary_string(payload->node, &str, &len);
    
    if( str != NULL ) {
      _RETVAL_STRINGL(str, len);
      efree(str);
    }
    
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} MustacheAST::__toString */

/* {{{ proto void MustacheAST::__wakeup() */
#if PHP_MAJOR_VERSION < 7
static inline void php_mustache_ast_wakeup(zval * _this_zval, zval * return_value TSRMLS_DC)
{
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval TSRMLS_CC);
    
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
    char * prop_name;
    char * class_name;

    if( Z_OBJ_HT_P(_this_zval)->get_properties == NULL ) {
        return;
    }

    data_hash = Z_OBJ_HT_P(_this_zval)->get_properties(_this_zval TSRMLS_CC);
    data_count = zend_hash_num_elements(data_hash);

    if( data_hash == NULL ) {
        return;
    }

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
            	mustache_node_from_binary_string(&payload->node, Z_STRVAL_PP(data_entry), Z_STRLEN_PP(data_entry));
            }
        }
        zend_hash_move_forward_ex(data_hash, &data_pointer);
    }
}
#else
static inline void php_mustache_ast_wakeup(zval * _this_zval, zval * return_value TSRMLS_DC)
{
    zend_class_entry * _this_ce = Z_OBJCE_P(_this_zval);
    struct php_obj_MustacheAST * payload = php_mustache_ast_object_fetch_object(_this_zval TSRMLS_CC);

    HashTable * data_hash = NULL;
    long data_count = 0;
    ulong key_nindex = 0;
    zend_string * key;
    zval * data_entry = NULL;
    const char * prop_name;
    const char * class_name;

    if( Z_OBJ_HT_P(_this_zval)->get_properties == NULL ) {
        return;
    }

    data_hash = Z_OBJ_HT_P(_this_zval)->get_properties(_this_zval TSRMLS_CC);
    data_count = zend_hash_num_elements(data_hash);

    if( data_hash == NULL ) {
        return;
    }

    ZEND_HASH_FOREACH_KEY_VAL(data_hash, key_nindex, key, data_entry) {
        if( key ) {
            zend_unmangle_property_name(key, &class_name, &prop_name);
            if( strcmp(prop_name, "binaryString") == 0 && Z_TYPE_P(data_entry) == IS_STRING ) {
            	mustache_node_from_binary_string(&payload->node, Z_STRVAL_P(data_entry), Z_STRLEN_P(data_entry));
            }
        }    
    } ZEND_HASH_FOREACH_END();
}
#endif

PHP_METHOD(MustacheAST, __wakeup)
{
  try {
    // Check parameters
    zval * _this_zval = NULL;
    if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), (char *) "O", 
            &_this_zval, MustacheAST_ce_ptr) == FAILURE) {
      throw PhpInvalidParameterException();
    }
    
    php_mustache_ast_wakeup(getThis(), return_value TSRMLS_CC);
  } catch(...) {
    mustache_exception_handler(TSRMLS_C);
  }
}
/* }}} MustacheAST::__wakeup */

