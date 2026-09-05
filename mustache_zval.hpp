#ifndef PHP_MUSTACHE_ZVAL_HPP
#define PHP_MUSTACHE_ZVAL_HPP

#include "php_mustache.h"

class ZvalGuard {
  private:
    zval value;

  public:
    ZvalGuard()
    {
      ZVAL_UNDEF(&value);
    }

    ~ZvalGuard()
    {
      if( !Z_ISUNDEF(value) ) {
        zval_ptr_dtor(&value);
      }
    }

    ZvalGuard(const ZvalGuard&) = delete;
    ZvalGuard& operator=(const ZvalGuard&) = delete;

    zval * get()
    {
      return &value;
    }
};

#endif /* PHP_MUSTACHE_ZVAL_HPP */
