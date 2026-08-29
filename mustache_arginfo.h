/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d95fa7cc68a1501735ffab5e0aa27b5a36bfe3f4 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Mustache___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Mustache_getEscapeByDefault, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Mustache_getStartSequence, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Mustache_getStopSequence arginfo_class_Mustache_getStartSequence

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Mustache_setEscapeByDefault, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_MASK(0, escapeByDefault, MAY_BE_BOOL|MAY_BE_LONG, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Mustache_setStartSequence, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, startSequence, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Mustache_setStopSequence, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, stopSequence, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Mustache_parse, 0, 1, MustacheAST, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, tmpl, MustacheTemplate|MustacheAST, MAY_BE_STRING, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Mustache_render, 0, 2, IS_STRING, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, str, MustacheTemplate|MustacheAST, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO(0, vars, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, partials, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Mustache_tokenize, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, tmpl, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Mustache_debugDataStructure, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, vars, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#if defined(PHP_MUSTACHE_ARCHIVE_BENCHMARK)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Mustache_benchmarkSerializeArchive, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, tmpl, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, partials, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_MUSTACHE_ARCHIVE_BENCHMARK)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Mustache_benchmarkRenderArchive, 0, 2, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, archive, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, vars, IS_MIXED, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MustacheAST___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, vars, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_MustacheAST_fromBinary, 0, 1, IS_STATIC, 0)
	ZEND_ARG_TYPE_INFO(0, binary, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_MustacheAST___sleep, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MustacheAST_toArray arginfo_class_MustacheAST___sleep

#define arginfo_class_MustacheAST_toBinary arginfo_class_Mustache_getStartSequence

#define arginfo_class_MustacheAST___toString arginfo_class_Mustache_getStartSequence

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_MustacheAST___wakeup, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MustacheTemplate___construct arginfo_class_MustacheAST___construct

#define arginfo_class_MustacheTemplate___toString arginfo_class_Mustache_getStartSequence

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_MustacheData___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, tmpl, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_MustacheData_toValue, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_MustacheLambdaHelper___construct arginfo_class_Mustache___construct

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_MustacheLambdaHelper_render, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, tmpl, IS_STRING, 0)
ZEND_END_ARG_INFO()
