
#ifndef PHP_MUSTACHE_EXCEPTIONS_HPP
#define PHP_MUSTACHE_EXCEPTIONS_HPP

#include <exception>
#include <stdexcept>

extern zend_class_entry * MustacheException_ce_ptr;
extern zend_class_entry * MustacheParserException_ce_ptr;

PHP_MINIT_FUNCTION(mustache_exceptions);

void mustache_exception_handler();

class PhpInvalidParameterException : public std::exception {
  public:
};

class InvalidParameterException : public std::runtime_error {
  public:
      explicit InvalidParameterException(const std::string& desc) : std::runtime_error(desc) { }
};

#endif /* PHP_MUSTACHE_EXCEPTIONS_HPP */
