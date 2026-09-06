#ifndef MUSTACHE_BAILOUT_BOUNDARY_PROTOTYPE_HPP
#define MUSTACHE_BAILOUT_BOUNDARY_PROTOTYPE_HPP

// Standalone design prototype: no Zend calls, jump handling, or PHP values.
#include <exception>
#include <string>
#include <utility>

namespace bailout_prototype {

enum class BoundaryStatus { completed, abort_request };
enum class OutcomeStatus { completed, failed, aborted };

class Request {
  public:
    Request() = default;
    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;

    void checkpoint(BoundaryStatus status = BoundaryStatus::completed)
    {
      aborted_ = aborted_ || status == BoundaryStatus::abort_request;
      if( aborted_ ) {
        throw AbortSignal{};
      }
    }

    void noteCleanup(BoundaryStatus status) noexcept
    {
      if( status == BoundaryStatus::abort_request ) {
        cleanupAborted_ = true;
        aborted_ = true;
      }
    }

    bool aborted() const noexcept { return aborted_; }
    bool cleanupAborted() const noexcept { return cleanupAborted_; }

  private:
    struct AbortSignal {};
    bool aborted_ = false;
    bool cleanupAborted_ = false;
};

class Cleanup {
  public:
    using Release = BoundaryStatus (*)(void *) noexcept;

    Cleanup(Request& request, Release release, void * value) noexcept :
        request_(request), release_(release), value_(value)
    {}

    ~Cleanup() noexcept { request_.noteCleanup(release_(value_)); }
    Cleanup(const Cleanup&) = delete;
    Cleanup& operator=(const Cleanup&) = delete;

  private:
    Request& request_;
    Release release_;
    void * value_;
};

struct Outcome {
    OutcomeStatus status;
    std::string output;
    std::exception_ptr failure;
};

template <typename Render> Outcome run(Request& request, Render&& render)
{
  std::string output;
  std::exception_ptr failure;
  try {
    request.checkpoint();
    output = std::forward<Render>(render)();
    request.checkpoint();
  } catch(...) {
    failure = std::current_exception();
  }
  if( request.aborted() ) {
    return {OutcomeStatus::aborted, {}, failure};
  }
  if( failure ) {
    return {OutcomeStatus::failed, {}, failure};
  }
  return {OutcomeStatus::completed, std::move(output), {}};
}

} // namespace bailout_prototype

#endif
