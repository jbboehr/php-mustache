
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define NOMINMAX

#include "php_mustache.h"
#include "mustache_private.hpp"
#include <algorithm>
#include "mustache_exceptions.hpp"
#include "mustache_lambda_helper.hpp"
#include "mustache_lambda.hpp"

#undef min

namespace {

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

class ZvalArguments {
  private:
    zval * values;
    int count;

  public:
    explicit ZvalArguments(int argument_count) :
        values(NULL),
        count(argument_count)
    {
      if( count == 0 ) {
        return;
      }

      values = (zval *) safe_emalloc(count, sizeof(zval), 0);
      for( int i = 0; i < count; i++ ) {
        ZVAL_UNDEF(&values[i]);
      }
    }

    ~ZvalArguments()
    {
      if( values == NULL ) {
        return;
      }

      for( int i = count - 1; i >= 0; i-- ) {
        if( !Z_ISUNDEF(values[i]) ) {
          zval_ptr_dtor(&values[i]);
        }
      }
      efree(values);
    }

    ZvalArguments(const ZvalArguments&) = delete;
    ZvalArguments& operator=(const ZvalArguments&) = delete;

    zval * data()
    {
      return values;
    }

    zval& operator[](int index)
    {
      return values[index];
    }
};

} // namespace

std::string Lambda::invokeUserFunctionAsString(int param_count, zval params[])
{
  ZvalGuard result;
  if( invokeUserFunction(result.get(), param_count, params) != SUCCESS ||
      EG(exception) != NULL || Z_ISUNDEF_P(result.get()) ) {
    return std::string();
  }

  convert_to_string(result.get());
  if( EG(exception) != NULL || Z_TYPE_P(result.get()) != IS_STRING ) {
    return std::string();
  }

  return std::string(Z_STRVAL_P(result.get()), Z_STRLEN_P(result.get()));
}

std::string Lambda::invoke()
{
  return invokeUserFunctionAsString(0, NULL);
}

std::string Lambda::invoke(
    std::string_view text, mustache::LambdaRenderContext context)
{
  int param_count = std::max(0, std::min(getUserFunctionParamCount(), 2));
  ZvalArguments params(param_count);
  if( param_count >= 1 ) {
    ZVAL_STRINGL(&params[0], text.data(), text.size());
  }
  if( param_count >= 2 ) {
    object_init_ex(&params[1], MustacheLambdaHelper_ce_ptr);

    struct php_obj_MustacheLambdaHelper * payload = php_mustache_lambda_helper_object_fetch_object(&params[1]);
    if( payload->state == NULL ) {
      throw InvalidParameterException("MustacheLambdaHelper state was not initialized properly");
    }
    payload->state->context = context;
  }

  return invokeUserFunctionAsString(param_count, params.data());
}
