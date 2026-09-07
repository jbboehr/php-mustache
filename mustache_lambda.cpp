
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define NOMINMAX

#include "php_mustache.h"
#include <algorithm>
#include <utility>
#include "mustache_exceptions.hpp"
#include "mustache_lambda_helper.hpp"
#include "mustache_lambda.hpp"
#include "mustache_lambda_result.hpp"
#include "mustache_zval.hpp"

#undef min

namespace {

class ZvalArguments {
  private:
    zval values[2];
    int count;

  public:
    explicit ZvalArguments(int argument_count) :
        count(argument_count)
    {
      for( int i = 0; i < count; i++ ) {
        ZVAL_UNDEF(&values[i]);
      }
    }

    ~ZvalArguments()
    {
      for( int i = count - 1; i >= 0; i-- ) {
        if( !Z_ISUNDEF(values[i]) ) {
          zval_ptr_dtor(&values[i]);
        }
      }
    }

    ZvalArguments(const ZvalArguments&) = delete;
    ZvalArguments& operator=(const ZvalArguments&) = delete;

    zval * data()
    {
      return count == 0 ? NULL : values;
    }

    zval& operator[](int index)
    {
      return values[index];
    }
};

} // namespace

mustache::LambdaResult Lambda::invokeUserFunctionAsResult(int param_count, zval params[])
{
  ZvalGuard result;
  int status = invokeUserFunction(result.get(), param_count, params);
  if( EG(exception) != NULL ) {
    throw PhpInvalidParameterException();
  }
  if( status != SUCCESS || Z_ISUNDEF_P(result.get()) ) {
    return mustache::LambdaResult::fromString(std::string());
  }

  zval * value = result.get();
  ZVAL_DEREF(value);
  if( Z_TYPE_P(value) == IS_OBJECT && php_mustache_is_lambda_result(Z_OBJCE_P(value)) ) {
    zend_string * text = php_mustache_lambda_result_text(value);
    std::string owned_text(ZSTR_VAL(text), ZSTR_LEN(text));
    return Z_OBJCE_P(value) == MustacheLiteralResult_ce_ptr
        ? mustache::LambdaResult::literal(std::move(owned_text))
        : mustache::LambdaResult::templateSource(std::move(owned_text));
  }

  convert_to_string(result.get());
  if( EG(exception) != NULL ) {
    throw PhpInvalidParameterException();
  }
  if( Z_TYPE_P(result.get()) != IS_STRING ) {
    return mustache::LambdaResult::fromString(std::string());
  }

  return mustache::LambdaResult::fromString(
      std::string(Z_STRVAL_P(result.get()), Z_STRLEN_P(result.get())));
}

mustache::LambdaResult Lambda::invokeResult()
{
  return invokeUserFunctionAsResult(0, NULL);
}

mustache::LambdaResult Lambda::invokeResult(
    std::string_view text, mustache::LambdaRenderContext context)
{
  int param_count = std::clamp(getUserFunctionParamCount(), 0, 2);
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

  return invokeUserFunctionAsResult(param_count, params.data());
}
