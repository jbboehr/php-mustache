
#ifndef PHP_MUSTACHE_HPP
#define PHP_MUSTACHE_HPP

extern "C" {

#define PHP_MUSTACHE_NAME "mustache"  
#define PHP_MUSTACHE_VERSION "0.1.0"
#define PHP_MUSTACHE_RELEASE "2012-03-14"
#define PHP_MUSTACHE_BUILD "master"
#define PHP_MUSTACHE_AUTHORS "John Boehr <jbboehr@gmail.com> (lead)"

#ifndef PHP_MUSTACHE_THROW_EXCEPTIONS
#define PHP_MUSTACHE_THROW_EXCEPTIONS 0
#endif
  
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>

#include <php_ini.h>
#include <SAPI.h>
#include <ext/standard/info.h>
#include <Zend/zend_extensions.h>
  
#ifdef PHP_WIN32
#define PHP_FFTW_API __declspec(dllexport)
#else
#define PHP_FFTW_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

} // extern "C" 

#include <exception>
#include <stdexcept>
#include <string>

#include "mustache/mustache.hpp"

// Module entry

extern zend_module_entry mustache_module_entry;
#define phpext_mustache_ptr &mustache_module_entry

// Module init/info

PHP_MINIT_FUNCTION(mustache);
PHP_MINFO_FUNCTION(mustache);

// Parameter exception
class PhpInvalidParameterException : public std::exception {
  public:
};
class InvalidParameterException : public std::runtime_error {
  public:
      InvalidParameterException(const std::string& desc) : std::runtime_error(desc) { }
};

// Utils

void mustache_node_to_zval(mustache::Node * node, zval * current TSRMLS_DC);
void mustache_data_from_zval(mustache::Data * node, zval * current TSRMLS_DC);
zval * mustache_data_to_zval(mustache::Data * node TSRMLS_DC);
zend_class_entry * mustache_get_class_entry(char * name, int len TSRMLS_DC);
void mustache_error_handler(const char * msg, mustache::Exception * e, 
        zval * return_value TSRMLS_DC);
void mustache_exception_handler();

bool mustache_parse_template_param(zval * tmpl, mustache::Mustache * mustache,
        mustache::Node ** node TSRMLS_DC);
bool mustache_parse_data_param(zval * data, mustache::Mustache * mustache,
        mustache::Data ** node TSRMLS_DC);
bool mustache_parse_partials_param(zval * array, mustache::Mustache * mustache,
        mustache::Node::Partials * partials TSRMLS_DC);

#endif /* PHP_MUSTACHE_HPP */