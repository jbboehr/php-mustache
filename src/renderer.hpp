
#ifndef MUSTACHE_RENDERER_HPP
#define MUSTACHE_RENDERER_HPP

#include <map>
#include <string>

#include "data.hpp"
#include "exception.hpp"
#include "node.hpp"
#include "utils.hpp"

namespace Mustache {


class Renderer {
  private:
    void _renderNode(Node * node, Stack * dataStack, std::string * output);
    
  public:
    static const int outputBufferLength = 100000;
    
    void render(Node * root, Data * data, std::string * output);
};


} // namespace Mustache

#endif
