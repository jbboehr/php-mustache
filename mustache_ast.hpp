
#ifndef PHP_MUSTACHE_AST_HPP
#define PHP_MUSTACHE_AST_HPP

#include <mustache/mustache.hpp>
#include <memory>
#include <string>

struct php_mustache_ast_state {
    std::unique_ptr<mustache::Node> node;
};

struct php_obj_MustacheAST {
    php_mustache_ast_state * state;
    zend_object std;
};

extern zend_class_entry * MustacheAST_ce_ptr;

std::unique_ptr<mustache::Node> mustache_node_from_binary_string(const char * str, size_t len);
std::string mustache_node_to_binary_string(const mustache::Node& node);
void mustache_node_to_zval(const mustache::Node& node, zval * current);

struct php_obj_MustacheAST * php_mustache_ast_object_fetch_object(zval * zv);

PHP_MINIT_FUNCTION(mustache_ast);

PHP_METHOD(MustacheAST, __construct);
PHP_METHOD(MustacheAST, __sleep);
PHP_METHOD(MustacheAST, toArray);
PHP_METHOD(MustacheAST, __toString);
PHP_METHOD(MustacheAST, __wakeup);

#endif /* PHP_MUSTACHE_AST_HPP */
