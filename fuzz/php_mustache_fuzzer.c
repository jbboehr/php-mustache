#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <main/php.h>
#include <zend_execute.h>
#include <zend_exceptions.h>

#include "fuzzer-sapi.h"

#define PHP_MUSTACHE_FUZZ_MAX_INPUT (4 * 1024)
#define PHP_MUSTACHE_FUZZ_LAMBDA \
    "static function ($text, $helper) { " \
    "return $helper->render('{{value}}'); " \
    "}"

static zend_class_entry *mustache_ce;

static void destroy_zval(zval *value)
{
    if (!Z_ISUNDEF_P(value)) {
        zval_ptr_dtor(value);
        ZVAL_UNDEF(value);
    }
}

static bool call_method(
    zval *object, const char *method, uint32_t argument_count, zval *arguments, zval *return_value)
{
    zval callable;
    bool threw;
    int status;

    array_init_size(&callable, 2);
    Z_TRY_ADDREF_P(object);
    add_next_index_zval(&callable, object);
    add_next_index_string(&callable, method);

    ZVAL_UNDEF(return_value);
    status = call_user_function(
        CG(function_table), NULL, &callable, return_value, argument_count, arguments);
    zval_ptr_dtor(&callable);

    threw = EG(exception) != NULL;
    if (threw) {
        zend_clear_exception();
    }

    return status == SUCCESS && !threw;
}

static void require_type(zval *value, int first, int second)
{
    if (Z_TYPE_P(value) != first && Z_TYPE_P(value) != second) {
        abort();
    }
}

static void initialize_data(zval *data, const char *input, size_t size)
{
    zval items;
    zval lambda;

    array_init(data);
    add_assoc_string(data, "name", "fuzz");
    add_assoc_stringl(data, "value", input, size);
    add_assoc_bool(data, "show", true);
    add_assoc_bool(data, "missing", false);

    array_init(&items);
    add_next_index_string(&items, "first");
    add_next_index_stringl(&items, input, size);
    add_assoc_zval(data, "items", &items);

    ZVAL_UNDEF(&lambda);
    if (zend_eval_stringl(
            PHP_MUSTACHE_FUZZ_LAMBDA,
            sizeof(PHP_MUSTACHE_FUZZ_LAMBDA) - 1,
            &lambda,
            "mustache fuzz lambda") == FAILURE ||
        Z_TYPE(lambda) != IS_OBJECT) {
        destroy_zval(&lambda);
        abort();
    }
    add_assoc_zval(data, "lambda", &lambda);
}

static void exercise_input(const uint8_t *bytes, size_t size)
{
    const char *input = size == 0 ? "" : (const char *) bytes;
    zval mustache;
    zval source_arguments[3];
    zval tokenize_result;
    zval parse_result;
    zval source_result;
    zval ast_result;
    bool source_rendered;
    bool ast_rendered = false;

    ZVAL_UNDEF(&mustache);
    ZVAL_UNDEF(&tokenize_result);
    ZVAL_UNDEF(&parse_result);
    ZVAL_UNDEF(&source_result);
    ZVAL_UNDEF(&ast_result);

    if (object_init_ex(&mustache, mustache_ce) != SUCCESS) {
        abort();
    }

    ZVAL_STRINGL(&source_arguments[0], input, size);
    initialize_data(&source_arguments[1], input, size);
    array_init(&source_arguments[2]);
    add_assoc_stringl(&source_arguments[2], "part", input, size);

    if (call_method(&mustache, "tokenize", 1, source_arguments, &tokenize_result)) {
        require_type(&tokenize_result, IS_ARRAY, IS_FALSE);
    }

    if (call_method(&mustache, "parse", 1, source_arguments, &parse_result)) {
        require_type(&parse_result, IS_OBJECT, IS_FALSE);
    }

    source_rendered = call_method(
        &mustache, "render", 3, source_arguments, &source_result);
    if (source_rendered) {
        require_type(&source_result, IS_STRING, IS_FALSE);
    }

    if (Z_TYPE(parse_result) == IS_OBJECT) {
        zval ast_arguments[3];

        ZVAL_COPY(&ast_arguments[0], &parse_result);
        ZVAL_COPY(&ast_arguments[1], &source_arguments[1]);
        ZVAL_COPY(&ast_arguments[2], &source_arguments[2]);
        ast_rendered = call_method(
            &mustache, "render", 3, ast_arguments, &ast_result);
        zval_ptr_dtor(&ast_arguments[0]);
        zval_ptr_dtor(&ast_arguments[1]);
        zval_ptr_dtor(&ast_arguments[2]);

        if (source_rendered != ast_rendered) {
            abort();
        }

        if (ast_rendered) {
            require_type(&ast_result, IS_STRING, IS_FALSE);
        }

        if (source_rendered) {
            if (Z_TYPE(source_result) != Z_TYPE(ast_result)) {
                abort();
            }
            if (Z_TYPE(source_result) == IS_STRING &&
                !zend_string_equals(Z_STR(source_result), Z_STR(ast_result))) {
                abort();
            }
        }
    }

    {
        zval reuse_arguments[3];
        zval reuse_result;
        bool reused;

        ZVAL_STRING(&reuse_arguments[0], "{{value}}");
        array_init(&reuse_arguments[1]);
        add_assoc_string(&reuse_arguments[1], "value", "safe");
        array_init(&reuse_arguments[2]);
        ZVAL_UNDEF(&reuse_result);

        reused = call_method(
            &mustache, "render", 3, reuse_arguments, &reuse_result);
        if (!reused || Z_TYPE(reuse_result) != IS_STRING ||
            !zend_string_equals_literal(Z_STR(reuse_result), "safe")) {
            abort();
        }

        destroy_zval(&reuse_result);
        zval_ptr_dtor(&reuse_arguments[0]);
        zval_ptr_dtor(&reuse_arguments[1]);
        zval_ptr_dtor(&reuse_arguments[2]);
    }

    destroy_zval(&ast_result);
    destroy_zval(&source_result);
    destroy_zval(&parse_result);
    destroy_zval(&tokenize_result);
    zval_ptr_dtor(&source_arguments[0]);
    zval_ptr_dtor(&source_arguments[1]);
    zval_ptr_dtor(&source_arguments[2]);
    destroy_zval(&mustache);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size > PHP_MUSTACHE_FUZZ_MAX_INPUT) {
        return 0;
    }

    if (fuzzer_request_startup() == FAILURE) {
        return 0;
    }
    fuzzer_setup_dummy_frame();

    zend_first_try {
        exercise_input(data, size);
    } zend_end_try();

    fuzzer_request_shutdown();
    return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    const char *extension = getenv("MUSTACHE_FUZZ_EXTENSION");
    char *configuration;
    size_t configuration_size;

    (void) argc;
    (void) argv;

    if (extension == NULL || extension[0] == '\0' ||
        strchr(extension, '\n') != NULL || strchr(extension, '\r') != NULL) {
        fprintf(stderr, "MUSTACHE_FUZZ_EXTENSION must name the mustache extension\n");
        abort();
    }

    configuration_size = strlen(extension) + sizeof("extension=\n");
    configuration = malloc(configuration_size);
    if (configuration == NULL) {
        abort();
    }
    snprintf(configuration, configuration_size, "extension=%s", extension);

    putenv("USE_TRACKED_ALLOC=1");
    if (fuzzer_init_php(configuration) == FAILURE) {
        free(configuration);
        abort();
    }
    free(configuration);

    mustache_ce = zend_hash_str_find_ptr(
        CG(class_table), "mustache", sizeof("mustache") - 1);
    if (mustache_ce == NULL) {
        fprintf(stderr, "Mustache class was not registered\n");
        abort();
    }

    return 0;
}
