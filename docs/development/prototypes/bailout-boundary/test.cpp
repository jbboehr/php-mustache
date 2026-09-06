#include "boundary.hpp"

#include <mustache/mustache.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>

using namespace bailout_prototype;

namespace {

void require(bool condition, const char * message)
{
  if( !condition ) {
    throw std::runtime_error(message);
  }
}

class Callback final : public mustache::Lambda {
  public:
    using Function = std::function<std::string(mustache::LambdaRenderContext)>;
    explicit Callback(Function function) : function_(std::move(function)) {}
    std::string invoke() override { return function_({}); }
    std::string invoke(std::string_view, mustache::LambdaRenderContext context) override
    {
      return function_(context);
    }

  private:
    Function function_;
};

mustache::Data callback(Callback::Function function)
{
  return mustache::Data::lambda(std::make_unique<Callback>(std::move(function)));
}

struct Script {
    BoundaryStatus invocation = BoundaryStatus::completed;
    BoundaryStatus conversion = BoundaryStatus::completed;
    BoundaryStatus cleanup = BoundaryStatus::completed;
    std::exception_ptr failure;
    int entered = 0;
    int converted = 0;
    int acquired = 0;
    int live = 0;
    int destroyed = 0;
    int released = 0;
};

// A native owner, not a model of Zend reference counting.
struct NativeValue {
    explicit NativeValue(Script& script) : script(script)
    {
      ++script.acquired;
      ++script.live;
    }
    ~NativeValue() noexcept
    {
      --script.live;
      ++script.destroyed;
    }
    NativeValue(const NativeValue&) = delete;
    NativeValue& operator=(const NativeValue&) = delete;
    Script& script;
};

BoundaryStatus release(void * value) noexcept
{
  auto& script = *static_cast<Script *>(value);
  ++script.released;
  return script.cleanup;
}

std::string invoke(Request& request, Script& script)
{
  request.checkpoint();
  std::string result;
  {
    auto value = std::make_unique<NativeValue>(script);
    Cleanup cleanup(request, release, &script);
    ++script.entered;
    request.checkpoint(script.invocation);
    if( script.failure ) {
      std::rethrow_exception(script.failure);
    }
    ++script.converted;
    request.checkpoint(script.conversion);
    result = "Ada";
  }
  // Cleanup can report an abort even when invocation and conversion succeeded.
  request.checkpoint();
  return result;
}

class Renderer {
  public:
    explicit Renderer(bool compiled) : compiled_(compiled) {}

    std::string render(std::string_view source, const mustache::Data& data)
    {
      if( compiled_ ) {
        return engine_.render(engine_.compile(source), data);
      }
      mustache::Node node;
      engine_.tokenize(source, &node);
      std::string output;
      engine_.render(&node, &data, nullptr, &output);
      return output;
    }

  private:
    bool compiled_;
    mustache::Mustache engine_;
};

void checkReleased(const Script& script)
{
  require(script.acquired == 1, "owner was not acquired before failure");
  require(script.live == 0 && script.destroyed == 1, "native owner was not released exactly once");
  require(script.released == 1, "cleanup boundary did not run exactly once");
}

void checkInactive(const mustache::LambdaRenderContext& context)
{
  require(!context.active(), "retained helper is still active");
  mustache::Node node;
  mustache::Tokenizer tokenizer;
  tokenizer.tokenize(std::string_view("{{name}}"), &node);
  bool rejected = false;
  try {
    (void) context.render(node);
  } catch( const mustache::Exception& ) {
    rejected = true;
  }
  require(rejected, "retained helper accepted rendering");
}

void checkRecovery(Renderer& renderer, Request& request)
{
  auto data = mustache::Data::object({{"name", mustache::Data::string("Grace")}});
  const auto outcome = run(request, [&]() { return renderer.render("Hello {{name}}", data); });
  require(outcome.status == OutcomeStatus::completed && outcome.output == "Hello Grace",
      "subsequent benign render failed");
}

void simpleCase(bool compiled, Script script, OutcomeStatus expected, int conversions, bool section = true)
{
  Renderer renderer(compiled);
  Request request;
  mustache::LambdaRenderContext retained;
  int later = 0;
  auto data = mustache::Data::object({
    {"first", callback([&](auto context) {
      retained = context;
      return invoke(request, script);
    })},
    {"later", callback([&](auto) { ++later; return "ok"; })},
  });
  const auto outcome = run(request, [&]() {
    return renderer.render(section ? "prefix{{#first}}unused{{/first}}|{{later}}" : "prefix{{first}}|{{later}}", data);
  });
  require(outcome.status == expected, "wrong terminal outcome");
  require(outcome.output == (expected == OutcomeStatus::completed ? "prefixAda|ok" : ""),
      "incorrect output publication");
  require(script.entered == 1 && script.converted == conversions, "incorrect callback/conversion progress");
  require(later == (expected == OutcomeStatus::completed ? 1 : 0), "later callback ran after failure");
  require(request.cleanupAborted() == (script.cleanup == BoundaryStatus::abort_request),
      "cleanup status was lost");
  if( script.failure ) {
    require(outcome.failure == script.failure, "original exception was replaced during cleanup");
  }
  checkReleased(script);
  if( section ) {
    checkInactive(retained);
  }
  Request fresh;
  checkRecovery(renderer, expected == OutcomeStatus::aborted ? fresh : request);
}

void nestedCase(bool compiled, bool conversionFailure)
{
  Renderer renderer(compiled);
  Request request;
  Script script;
  if( conversionFailure ) {
    script.conversion = BoundaryStatus::abort_request;
  } else {
    script.invocation = BoundaryStatus::abort_request;
  }
  mustache::LambdaRenderContext outer, inner;
  mustache::Node nested;
  mustache::Tokenizer tokenizer;
  tokenizer.tokenize(std::string_view("{{#inner}}unused{{/inner}}"), &nested);
  int afterInner = 0;
  auto data = mustache::Data::object({
    {"outer", callback([&](auto context) {
      outer = context;
      const auto result = context.render(nested);
      ++afterInner;
      return result;
    })},
    {"inner", callback([&](auto context) {
      inner = context;
      return invoke(request, script);
    })},
  });
  const auto outcome = run(request, [&]() { return renderer.render("{{#outer}}unused{{/outer}}", data); });
  require(outcome.status == OutcomeStatus::aborted && outcome.output.empty(), "nested abort was swallowed");
  require(afterInner == 0, "outer callback resumed after nested abort");
  require(script.entered == 1 && script.converted == (conversionFailure ? 1 : 0), "nested progress is incorrect");
  checkReleased(script);
  checkInactive(inner);
  checkInactive(outer);
  Request fresh;
  checkRecovery(renderer, fresh);
}

void terminalCases()
{
  Request request;
  try {
    request.checkpoint(BoundaryStatus::abort_request);
  } catch(...) {
    // A nested caller consuming the marker must not reset the request state.
  }
  int entered = 0;
  auto outcome = run(request, [&]() { ++entered; return std::string("published"); });
  require(outcome.status == OutcomeStatus::aborted && outcome.output.empty() && entered == 0,
      "an aborted request resumed work");

  Script skipped;
  try {
    (void) invoke(request, skipped);
  } catch(...) {
  }
  require(skipped.entered == 0 && skipped.acquired == 0 && skipped.converted == 0,
      "an aborted request reentered a callback boundary");

  Request cleanupRequest;
  Script script;
  script.cleanup = BoundaryStatus::abort_request;
  outcome = run(cleanupRequest, [&]() {
    auto value = std::make_unique<NativeValue>(script);
    Cleanup cleanup(cleanupRequest, release, &script);
    return std::string("must not publish");
  });
  require(outcome.status == OutcomeStatus::aborted && outcome.output.empty(),
      "final cleanup abort allowed output publication");
  checkReleased(script);
  cleanupRequest.noteCleanup(BoundaryStatus::completed);
  require(cleanupRequest.aborted() && cleanupRequest.cleanupAborted(),
      "successful cleanup cleared an earlier abort");
}

} // namespace

int main()
{
  const char * current = "terminal status";
  try {
    terminalCases();
    std::cout << "terminal status: passed\n";
    for( bool compiled : {false, true} ) {
      const char * mode = compiled ? "compiled" : "AST";
      auto test = [&](const char * name, const auto& body) {
        current = name;
        body();
        std::cout << mode << '/' << name << ": passed\n";
      };
      test("success", [&]() { simpleCase(compiled, {}, OutcomeStatus::completed, 1); });
      Script invocation;
      invocation.invocation = BoundaryStatus::abort_request;
      test("callback abort", [&]() { simpleCase(compiled, invocation, OutcomeStatus::aborted, 0); });
      test("interpolation abort", [&]() { simpleCase(compiled, invocation, OutcomeStatus::aborted, 0, false); });
      Script conversion;
      conversion.conversion = BoundaryStatus::abort_request;
      test("conversion abort", [&]() { simpleCase(compiled, conversion, OutcomeStatus::aborted, 1); });
      Script cleanup;
      cleanup.cleanup = BoundaryStatus::abort_request;
      test("cleanup abort", [&]() { simpleCase(compiled, cleanup, OutcomeStatus::aborted, 1); });
      Script failure;
      failure.failure = std::make_exception_ptr(std::runtime_error("original callback exception"));
      test("ordinary exception", [&]() { simpleCase(compiled, failure, OutcomeStatus::failed, 0); });
      Script nonstandard;
      nonstandard.failure = std::make_exception_ptr(17);
      test("nonstandard exception", [&]() { simpleCase(compiled, nonstandard, OutcomeStatus::failed, 0); });
      failure.cleanup = BoundaryStatus::abort_request;
      test("cleanup during exception", [&]() { simpleCase(compiled, failure, OutcomeStatus::aborted, 0); });
      invocation.cleanup = BoundaryStatus::abort_request;
      test("cleanup during abort", [&]() { simpleCase(compiled, invocation, OutcomeStatus::aborted, 0); });
      test("nested callback abort", [&]() { nestedCase(compiled, false); });
      test("nested conversion abort", [&]() { nestedCase(compiled, true); });
    }
  } catch( const std::exception& error ) {
    std::cerr << current << ": " << error.what() << '\n';
    return 1;
  } catch(...) {
    std::cerr << current << ": unexpected nonstandard exception\n";
    return 1;
  }
}
