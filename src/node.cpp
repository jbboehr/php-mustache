
#include "node.hpp"

namespace mustache {


Node::~Node()
{
  // Data
  delete data;
  
  // Children
  if( children.size() > 0 ) {
    Node::Children::iterator it;
    for ( it = children.begin() ; it != children.end(); it++ ) {
      delete *it;
    }
  }
  children.clear();
}


} // namespace Mustache
