
#include <exception>
#include <stdexcept>
#include <string>

#ifndef MUSTACHE_EXCEPTION_HPP
#define MUSTACHE_EXCEPTION_HPP

namespace mustache {


class Exception : public std::runtime_error {
  public:
      Exception(const std::string& desc) : std::runtime_error(desc) { }
};


} // namespace Mustache

#endif


