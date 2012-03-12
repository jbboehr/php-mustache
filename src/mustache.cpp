
#include "mustache.hpp"

namespace mustache {


void Mustache::tokenize(std::string * tmpl, Node * root)
{
  tokenizer.tokenize(tmpl, root);
}

void Mustache::render(std::string * tmpl, Data * data, std::string * output)
{
  Node root;
  
  // Tokenize
  tokenizer.tokenize(tmpl, &root);
  
  // Render
  renderer.render(&root, data, output);
}


} // namespace Mustache
