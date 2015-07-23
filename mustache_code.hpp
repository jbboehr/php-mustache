
#ifndef PHP_MUSTACHE_CODE_HPP
#define PHP_MUSTACHE_CODE_HPP

#ifdef __cplusplus
extern "C" {
#endif

extern zend_class_entry * MustacheCode_ce_ptr;

PHP_MINIT_FUNCTION(mustache_code);

PHP_METHOD(MustacheCode, __construct);
PHP_METHOD(MustacheCode, toReadableString);
PHP_METHOD(MustacheCode, __toString);

#ifdef __cplusplus
} // extern "C" 
#endif

#endif /* PHP_MUSTACHE_CODE_HPP */

