
#ifndef PHP_MUSTACHE_H
#define PHP_MUSTACHE_H

#include <string>

#define PHP_MUSTACHE_NAME "mustache"
#define PHP_MUSTACHE_VERSION "0.9.0"
#define PHP_MUSTACHE_RELEASE "2019-05-27"
#define PHP_MUSTACHE_BUILD "master"
#define PHP_MUSTACHE_AUTHORS "John Boehr <jbboehr@gmail.com> (lead)"
#define PHP_MUSTACHE_SPEC "1.1.2"

#ifndef PHP_MUSTACHE_THROW_EXCEPTIONS
#define PHP_MUSTACHE_THROW_EXCEPTIONS 0
#endif

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#ifdef __cplusplus
   extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define MUSTACHEG(v) TSRMG(mustache_globals_id, zend_mustache_globals *, v)
#else
#define MUSTACHEG(v) (mustache_globals.v)
#endif

extern zend_module_entry mustache_module_entry;
#define phpext_mustache_ptr &mustache_module_entry

ZEND_BEGIN_MODULE_GLOBALS(mustache)
	zend_bool  default_escape_by_default;
	char      *default_start_sequence;
	char      *default_stop_sequence;
ZEND_END_MODULE_GLOBALS(mustache)

ZEND_EXTERN_MODULE_GLOBALS(mustache);

#ifdef __cplusplus
  } // extern "C"
#endif

#endif	/* PHP_MUSTACHE_H */
