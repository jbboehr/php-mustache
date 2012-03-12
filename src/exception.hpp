
#ifndef MUSTACHE_EXCEPTION_HPP
#define MUSTACHE_EXCEPTION_HPP

#include <exception>
#include <stdexcept>
#include <string>

namespace Mustache {


class Exception : public std::runtime_error {
  public:
      MustacheException(const std::string& desc) : std::runtime_error(desc) { }
};


} // namespace Mustache

#endif


