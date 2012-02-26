
#include "php_mustache.hpp"
#include "php_mustache_methods.hpp"
#include "mustache.hpp"

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

/* {{{ proto string getStartSequence()
   */
PHP_METHOD(Mustache, getStartSequence)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;
  std::string str;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, Mustache_ce_ptr) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  str = payload->mustache->getStartSequence();
  
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
  std::string str;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, Mustache_ce_ptr) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  str = payload->mustache->getStopSequence();
  
  RETURN_STRING(str.c_str(), 1);
}
/* }}} getStopSequence */

/* {{{ proto boolean setStartSequence(string str)
   */
PHP_METHOD(Mustache, setStartSequence)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_Mustache *payload;
  
  char *str;
  long str_len;
  std::string cppstr;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &_this_zval, Mustache_ce_ptr, &str, &str_len) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  cppstr.assign(str, str_len);
  payload->mustache->setStartSequence(cppstr);
  
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
  
  char *str;
  long str_len;
  std::string cppstr;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &_this_zval, Mustache_ce_ptr, &str, &str_len) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  cppstr.assign(str, str_len);
  payload->mustache->setStopSequence(cppstr);
  
  RETURN_TRUE;
}
/* }}} setStartSequence */

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
  HashPosition data_pointer = NULL;
  zval **data_entry = NULL;
  long data_count;
  
  /*
  int data_curkey_ret;
  ulong data_curkey_npos;
  char *data_curkey;
  long data_curkey_len;
  char *data_curval;
  long data_curval_len;
  */
  
  string * template_str_obj;
  map<string,MustacheData> template_data;
  string * return_str;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Osa/", &_this_zval, Mustache_ce_ptr, &template_str, &template_len, &data) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  data_hash = HASH_OF(data);
  data_count = zend_hash_num_elements(data_hash);
  zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
  while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
    /*
    data_curkey_ret = zend_hash_get_current_key_ex(data_hash, &data_curkey, &data_curkey_len, &data_curkey_npos, true, data_entry);
    if( data_curkey_ret == HASH_KEY_IS_LONG ) {
      data_curkey_len = sprintf(data_curkey, "%ld", data_curkey_npos);
    }
    switch( Z_TYPE_PP(data_entry) ) {
      case IS_LONG:
      case IS_DOUBLE:
      case IS_STRING:
            convert_to_string(*data_entry);
            data_curval = Z_STRVAL_PP(data_entry);
            data_curval_len = Z_STRLEN_PP(data_entry);
            arr.insert(std::pair<*std::string, *std::string>(
                new std::string(data_curkey, data_curkey_len),
                new std::string(data_curval, data_curval_len))
            );
            break;
      default:
            php_error(E_WARNING, "Invalid data type");
            //RETURN_FALSE;
            break;
    }
    */
    zend_hash_move_forward_ex(data_hash, &data_pointer);
  }
  
  template_str_obj = new string(template_str, template_len);
  
  return_str = payload->mustache->render(template_str_obj, &template_data);
  
  RETURN_STRING(return_str->c_str(), 0); // Do reallocate
  //RETURN_STRINGL(return_str, return_len, 0); // Do not reallocate
}
/* }}} setStartSequence */