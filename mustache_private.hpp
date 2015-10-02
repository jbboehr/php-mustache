
#ifndef PHP_MUSTACHE_PRIVATE_HPP
#define PHP_MUSTACHE_PRIVATE_HPP

#ifdef __cplusplus
   extern "C" {
#endif

#include "php_mustache.h"

#include <stdint.h>

#if PHP_MAJOR_VERSION >= 7
#include <zend_string.h>
#endif

#if PHP_MAJOR_VERSION < 7
#define _add_next_index_string(...) add_next_index_string(__VA_ARGS__, 1)
#define _add_assoc_string(...) add_assoc_string(__VA_ARGS__, 1)
#define _add_assoc_string_ex(...) add_assoc_string_ex(__VA_ARGS__, 1)
#define _add_assoc_stringl_ex(...) add_assoc_stringl_ex(__VA_ARGS__, 1)
#define _RETURN_STRING(a) RETURN_STRING(a, 1)
#define _RETVAL_STRING(a) RETVAL_STRING(a, 1)
#define _RETVAL_STRINGL(a, b) RETVAL_STRINGL(a, b, 1)
#define _DECLARE_ZVAL(name) zval * name
#define _INIT_ZVAL INIT_ZVAL
#define _ALLOC_INIT_ZVAL(name) ALLOC_INIT_ZVAL(name)
#define _STRS ZEND_STRS
#define _zend_read_property(a, b, c, d, e, f) zend_read_property(a, b, c, d, e TSRMLS_CC)
#define _zend_register_internal_class_ex(class, parent) zend_register_internal_class_ex(class, parent, NULL TSRMLS_CC)
#define _ZVAL_STRINGL(a, b, c) ZVAL_STRINGL(a, b, c, 1)
typedef int strsize_t;
#else
#define _add_next_index_string add_next_index_string
#define _add_assoc_string(z, k, s) add_assoc_string_ex(z, k, strlen(k)+1, s)
#define _add_assoc_string_ex add_assoc_string_ex
#define _add_assoc_stringl_ex add_assoc_stringl_ex
#define _RETURN_STRING(a) RETURN_STRING(a)
#define _RETVAL_STRING(a) RETVAL_STRING(a)
#define _RETVAL_STRINGL RETVAL_STRINGL
#define _DECLARE_ZVAL(name) zval name ## _v; zval * name = &name ## _v
#define _INIT_ZVAL ZVAL_NULL
#define _ALLOC_INIT_ZVAL(name) ZVAL_NULL(name)
#define _STRS ZEND_STRL
#define _zend_read_property(a, b, c, d, e, f) zend_read_property(a, b, c, d, e, f)
#define _zend_register_internal_class_ex zend_register_internal_class_ex
#define _ZVAL_STRINGL ZVAL_STRINGL
typedef size_t strsize_t;
#endif

#ifdef __cplusplus
  } // extern "C" 
#endif

#include <exception>
#include <stdexcept>
#include <string>

#include "mustache.hpp"

#include "mustache_ast.hpp"
#include "mustache_code.hpp"
#include "mustache_data.hpp"
#include "mustache_exceptions.hpp"
#include "mustache_mustache.hpp"
#include "mustache_template.hpp"

#endif /* PHP_MUSTACHE_PRIVATE_HPP */

