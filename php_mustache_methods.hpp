
#ifndef PHP_MUSTACHE_METHODS_HPP
#define PHP_MUSTACHE_METHODS_HPP

extern "C" {
  #include <php.h>
}

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <utility>

#include "mustache/mustache.hpp"


using namespace std;

PHP_METHOD(Mustache, __construct);
ZEND_BEGIN_ARG_INFO_EX(Mustache____construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Mustache, getEscapeByDefault);
ZEND_BEGIN_ARG_INFO_EX(Mustache__getEscapeByDefault_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()
        
PHP_METHOD(Mustache, getStartSequence);
ZEND_BEGIN_ARG_INFO_EX(Mustache__getStartSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Mustache, getStopSequence);
ZEND_BEGIN_ARG_INFO_EX(Mustache__getStopSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Mustache, setEscapeByDefault);
ZEND_BEGIN_ARG_INFO_EX(Mustache__setEscapeByDefault_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, escapeByDefault)
ZEND_END_ARG_INFO()

PHP_METHOD(Mustache, setStartSequence);
ZEND_BEGIN_ARG_INFO_EX(Mustache__setStartSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, startSequence)
ZEND_END_ARG_INFO()

PHP_METHOD(Mustache, setStopSequence);
ZEND_BEGIN_ARG_INFO_EX(Mustache__setStopSequence_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, stopSequence)
ZEND_END_ARG_INFO()
        
PHP_METHOD(Mustache, compile);
ZEND_BEGIN_ARG_INFO_EX(Mustache__compile_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()
        
PHP_METHOD(Mustache, tokenize);
ZEND_BEGIN_ARG_INFO_EX(Mustache__tokenize_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, tmpl)
ZEND_END_ARG_INFO()
        
PHP_METHOD(Mustache, render);
ZEND_BEGIN_ARG_INFO_EX(Mustache__render_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 3)
	ZEND_ARG_INFO(0, str)
        ZEND_ARG_INFO(0, vars)
        ZEND_ARG_INFO(0, partials)
ZEND_END_ARG_INFO()
        
PHP_METHOD(Mustache, debugDataStructure);
ZEND_BEGIN_ARG_INFO_EX(Mustache__debugDataStructure_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
        ZEND_ARG_INFO(0, vars)
ZEND_END_ARG_INFO()

PHP_METHOD(MustacheTemplate, render);
ZEND_BEGIN_ARG_INFO_EX(MustacheTemplate__render_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
        ZEND_ARG_INFO(0, vars)
        ZEND_ARG_INFO(0, partials)
ZEND_END_ARG_INFO()

void mustache_node_to_zval(mustache::Node * node, zval * current);
void mustache_data_from_zval(mustache::Data * node, zval * current);
zval * mustache_data_to_zval(mustache::Data * node);
void mustache_partials_from_zval(mustache::Mustache * mustache, 
        mustache::Node::Partials * partials, zval * current);

#endif