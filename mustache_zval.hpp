#ifndef PHP_MUSTACHE_ZVAL_HPP
#define PHP_MUSTACHE_ZVAL_HPP

#include "php_mustache.h"

inline zval * mustache_dereference_zval(zval * value)
{
  if( value == NULL ) {
    return NULL;
  }
  if( Z_TYPE_P(value) == IS_INDIRECT ) {
    value = Z_INDIRECT_P(value);
  }
  ZVAL_DEREF(value);
  return value;
}

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

    ZvalGuard(ZvalGuard&& other) noexcept
    {
      ZVAL_COPY_VALUE(&value, &other.value);
      ZVAL_UNDEF(&other.value);
    }

    zval * get()
    {
      return &value;
    }
};

#endif /* PHP_MUSTACHE_ZVAL_HPP */
