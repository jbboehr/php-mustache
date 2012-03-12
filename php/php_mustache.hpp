
#ifndef PHP_MUSTACHE_HPP
#define PHP_MUSTACHE_HPP

extern "C" {

#define PHP_MUSTACHE_NAME "mustache"  
#define PHP_MUSTACHE_VERSION "0.0.1"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>

#include <php_ini.h>
#include <SAPI.h>
#include <ext/standard/info.h>
#include <Zend/zend_extensions.h>
  
} // extern "C" 

#include <string>

#include "mustache.hpp"
#include "php_mustache_methods.hpp"

extern "C" {

static zend_class_entry * Mustache_ce_ptr = NULL;
extern zend_module_entry mustache_module_entry;  
#define phpext_mustache_ptr &mustache_module_entry  

#ifdef PHP_WIN32
#define PHP_FFTW_API __declspec(dllexport)
#else
#define PHP_FFTW_API
#endif

PHP_MINIT_FUNCTION(mustache);
PHP_MINFO_FUNCTION(mustache);

#ifdef ZTS
#include "TSRM.h"
#endif

} // extern "C" 



// Profiler
// https://github.com/wadey/node-microtime/blob/master/src/microtime.cc
#if HAVE_MUSTACHE_PROFILER
#include <errno.h>
#include <sys/time.h>
#endif



// Class object structure

typedef struct _php_obj_Mustache {
    zend_object obj;
    mustache::Mustache * mustache;
} php_obj_Mustache;



#endif /* PHP_MUSTACHE_HPP */