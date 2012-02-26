
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
  
  char *template_str;
  long template_len;
  zval * data = NULL;
  HashTable * data_hash = NULL;
  char *return_str;
  long return_len = 0;

  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Osa/", &_this_zval, Mustache_ce_ptr, &template_str, &template_len, &data) == FAILURE) {
          return;
  }

  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_Mustache *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  
  RETURN_STRINGL(return_str, return_len, 0); // Do not reallocate
}
/* }}} setStartSequence */