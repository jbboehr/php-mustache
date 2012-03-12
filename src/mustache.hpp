
#ifndef MUSTACHE_HPP
#define MUSTACHE_HPP

#include <iostream>
#include <memory>
#include <string>

#include "data.hpp"
#include "exception.hpp"
#include "node.hpp"
#include "renderer.hpp"
#include "tokenizer.hpp"
#include "utils.hpp"

namespace Mustache {


class Mustache {
  private:
  public:
    typedef std::auto_ptr<Mustache> Ptr;
    
    Tokenizer tokenizer;
    Renderer renderer;
    
    void tokenize(std::string * tmpl, Node * root);
    void render(std::string * tmpl, Data * data, std::string * output);
};


} // namespace Mustache

#endif
