
#ifndef MUSTACHE_NODE_HPP
#define MUSTACHE_NODE_HPP

#include <memory>
#include <string>
#include <vector>

namespace Mustache {


class Node {
  public:
    typedef std::auto_ptr<Node> Ptr;
    typedef std::vector<Node *> Children;
    enum Type {
      TypeNone = 0,
      TypeRoot = 1,
      TypeOutput = 2,
      TypeTag = 3
    };
    enum Flag { 
      FlagNone = 0,
      FlagEscape = 1,
      FlagNegate = 2,
      FlagSection = 4,
      FlagStop = 8,
      FlagComment = 16,
      FlagPartial = 32,
      FlagInlinePartial = 64,
      
      FlagHasChildren = Node::FlagNegate | Node::FlagSection | Node::FlagPartial
    };
    
    Node::Type type;
    int flags;
    std::string * data;
    Node::Children children;
    
    ~Node();
};


} // namespace Mustache

#endif


