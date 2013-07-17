
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

typedef struct _php_obj_Mustache {
    zend_object obj;
    mustache::Mustache * mustache;
} php_obj_Mustache;

typedef struct _php_obj_MustacheData {
    zend_object obj;
    mustache::Data * data;
} php_obj_MustacheData;

typedef struct _php_obj_MustacheTemplate {
    zend_object obj;
    std::string * tmpl;
    mustache::Node * node;
} php_obj_MustacheTemplate;

void mustache_exception_handler(TSRMLS_D);

#ifdef __cplusplus
  } // extern "C" 
#endif

#endif /* PHP_MUSTACHE_HPP */
