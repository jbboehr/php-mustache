
#ifndef PHP_MUSTACHE_HPP
#define PHP_MUSTACHE_HPP

#include "php_mustache.h"

#include <exception>
#include <stdexcept>
#include <string>

#include "mustache/src/mustache.hpp"

// Parameter exception
class PhpInvalidParameterException : public std::exception {
  public:
};
class InvalidParameterException : public std::runtime_error {
  public:
      InvalidParameterException(const std::string& desc) : std::runtime_error(desc) { }
};

// Utils

#ifdef __cplusplus
   extern "C" {
#endif

void mustache_node_from_binary_string(mustache::Node ** node, char * str, int len);
void mustache_node_to_binary_string(mustache::Node * node, char ** estr, int * elen);
void mustache_node_to_zval(mustache::Node * node, zval * current TSRMLS_DC);
void mustache_data_from_zval(mustache::Data * node, zval * current TSRMLS_DC);
void mustache_data_to_zval(mustache::Data * node, zval * current TSRMLS_DC);
zend_class_entry * mustache_get_class_entry(char * name, int len TSRMLS_DC);
void mustache_exception_handler(TSRMLS_D);

bool mustache_parse_template_param(zval * tmpl, mustache::Mustache * mustache,
        mustache::Node ** node TSRMLS_DC);
bool mustache_parse_data_param(zval * data, mustache::Mustache * mustache,
        mustache::Data ** node TSRMLS_DC);
bool mustache_parse_partials_param(zval * array, mustache::Mustache * mustache,
        mustache::Node::Partials * partials TSRMLS_DC);

#ifdef __cplusplus
  } // extern "C" 
#endif

#endif /* PHP_MUSTACHE_HPP */
