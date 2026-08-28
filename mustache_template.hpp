
#ifndef PHP_MUSTACHE_TEMPLATE_HPP
#define PHP_MUSTACHE_TEMPLATE_HPP

extern zend_class_entry * MustacheTemplate_ce_ptr;

PHP_MINIT_FUNCTION(mustache_template);

PHP_METHOD(MustacheTemplate, __construct);
PHP_METHOD(MustacheTemplate, __toString);

#endif /* PHP_MUSTACHE_TEMPLATE_HPP */
