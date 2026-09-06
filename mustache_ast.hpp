
#ifndef PHP_MUSTACHE_AST_HPP
#define PHP_MUSTACHE_AST_HPP

#include <mustache/mustache.hpp>
#include <memory>
#include <optional>
#include <string>

struct php_mustache_ast_source {
    std::string text;
    mustache::Tokenizer tokenizer;
};

struct php_mustache_ast_state {
    std::unique_ptr<mustache::Node> node;
    // Source and parse-time settings let owned partial copies retain the
    // section text that Node's public fields cannot reproduce exactly.
    std::optional<php_mustache_ast_source> source;
};

struct php_obj_MustacheAST {
    php_mustache_ast_state * state;
    zend_object std;
};

extern zend_class_entry * MustacheAST_ce_ptr;

void mustache_node_to_zval(const mustache::Node& node, zval * current);

struct php_obj_MustacheAST * php_mustache_ast_object_fetch_object(zval * zv);

PHP_MINIT_FUNCTION(mustache_ast);

PHP_METHOD(MustacheAST, __construct);
PHP_METHOD(MustacheAST, fromBinary);
PHP_METHOD(MustacheAST, __serialize);
PHP_METHOD(MustacheAST, __unserialize);
PHP_METHOD(MustacheAST, toArray);
PHP_METHOD(MustacheAST, toBinary);

#endif /* PHP_MUSTACHE_AST_HPP */
