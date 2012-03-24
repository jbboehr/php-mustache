
#ifndef PHP_MUSTACHE_HPP
#define PHP_MUSTACHE_HPP

extern "C" {

#define PHP_MUSTACHE_NAME "mustache"  
#define PHP_MUSTACHE_VERSION "0.2.0"
#define PHP_MUSTACHE_RELEASE "2012-03-22"
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
#include <php_variables.h>
#include <php_globals.h>
#include <SAPI.h>
#include <ext/standard/info.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_extensions.h>

#ifdef ZTS
#include "TSRM.h"
#endif

} // extern "C" 

#include <exception>
#include <stdexcept>
#include <string>

#include "mustache/mustache.hpp"

// Parameter exception
class PhpInvalidParameterException : public std::exception {
  public:
};
class InvalidParameterException : public std::runtime_error {
  public:
      InvalidParameterException(const std::string& desc) : std::runtime_error(desc) { }
};

extern "C" {
  
// Module

extern "C" zend_module_entry mustache_module_entry;
#define phpext_mustache_ptr &mustache_module_entry

ZEND_BEGIN_MODULE_GLOBALS(mustache)
	zend_bool  default_escape_by_default;
	char      *default_start_sequence;
	char      *default_stop_sequence;
ZEND_END_MODULE_GLOBALS(mustache)
        
ZEND_EXTERN_MODULE_GLOBALS(mustache);

#ifdef ZTS
#define MUSTACHEG(v) TSRMG(mustache_globals_id, zend_mustache_globals *, v)
#else
#define MUSTACHEG(v) (mustache_globals.v)
#endif
        
// Utils

PHPAPI void mustache_node_to_zval(mustache::Node * node, zval * current TSRMLS_DC);
PHPAPI void mustache_data_from_zval(mustache::Data * node, zval * current TSRMLS_DC);
PHPAPI zval * mustache_data_to_zval(mustache::Data * node TSRMLS_DC);
PHPAPI zend_class_entry * mustache_get_class_entry(char * name, int len TSRMLS_DC);
PHPAPI void mustache_exception_handler(TSRMLS_D);

PHPAPI bool mustache_parse_template_param(zval * tmpl, mustache::Mustache * mustache,
        mustache::Node ** node TSRMLS_DC);
PHPAPI bool mustache_parse_data_param(zval * data, mustache::Mustache * mustache,
        mustache::Data ** node TSRMLS_DC);
PHPAPI bool mustache_parse_partials_param(zval * array, mustache::Mustache * mustache,
        mustache::Node::Partials * partials TSRMLS_DC);

} // extern "C" 


#endif /* PHP_MUSTACHE_HPP */