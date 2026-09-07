#ifndef PHP_MUSTACHE_LAMBDA_RESULT_HPP
#define PHP_MUSTACHE_LAMBDA_RESULT_HPP

extern zend_class_entry * MustacheLiteralResult_ce_ptr;
extern zend_class_entry * MustacheTemplateResult_ce_ptr;

inline bool php_mustache_is_lambda_result(zend_class_entry * ce)
{
  return ce == MustacheLiteralResult_ce_ptr || ce == MustacheTemplateResult_ce_ptr;
}

// Borrows the text while the result object is retained. Throws if uninitialized.
zend_string * php_mustache_lambda_result_text(zval * value);

PHP_MINIT_FUNCTION(mustache_lambda_result);

#endif /* PHP_MUSTACHE_LAMBDA_RESULT_HPP */
