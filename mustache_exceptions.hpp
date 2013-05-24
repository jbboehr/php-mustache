
#ifndef PHP_MUSTACHE_EXCEPTIONS_HPP
#define PHP_MUSTACHE_EXCEPTIONS_HPP


#include "php.h"


extern "C" zend_class_entry * MustacheException_ce_ptr;
extern "C" zend_class_entry * MustacheParserException_ce_ptr;


PHP_MINIT_FUNCTION(mustache_exceptions);


#endif
