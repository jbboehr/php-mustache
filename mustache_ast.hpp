
#ifndef PHP_MUSTACHE_AST_HPP
#define PHP_MUSTACHE_AST_HPP

#ifdef __cplusplus
extern "C" {
#endif

#if PHP_MAJOR_VERSION < 7
struct php_obj_MustacheAST {
    zend_object std;
    mustache::Node * node;
};
#else
struct php_obj_MustacheAST {
    mustache::Node * node;
    zend_object std;
};
#endif

extern zend_class_entry * MustacheAST_ce_ptr;

void mustache_node_from_binary_string(mustache::Node ** node, char * str, int len);
void mustache_node_to_binary_string(mustache::Node * node, char ** estr, int * elen);
void mustache_node_to_zval(mustache::Node * node, zval * current TSRMLS_DC);

struct php_obj_MustacheAST * php_mustache_ast_object_fetch_object(zval * zv TSRMLS_DC);

PHP_MINIT_FUNCTION(mustache_ast);

PHP_METHOD(MustacheAST, __construct);
PHP_METHOD(MustacheAST, __sleep);
PHP_METHOD(MustacheAST, toArray);
PHP_METHOD(MustacheAST, __toString);
PHP_METHOD(MustacheAST, __wakeup);

#ifdef __cplusplus
} // extern "C" 
#endif

#endif /* PHP_MUSTACHE_AST_HPP */

