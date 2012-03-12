
#include "renderer.hpp"

namespace mustache {


void Renderer::render(Node * root, Data * data, std::string * output)
{
  output->reserve(Renderer::outputBufferLength);
  
  Stack dataStack;
  dataStack.push(data);
  
  _renderNode(root, &dataStack, output);
}

void Renderer::_renderNode(Node * node, Stack * dataStack, std::string * output)
{
  Data * data = dataStack->top();
  std::string * nstr = node->data;
  
  if( data == NULL ) {
    throw Exception("Whoops, empty data");
  }
  
    // Handle simple cases
  if( node->type == Node::TypeRoot ) {
    if( node->children.size() > 0 ) {
      Node::Children::iterator it;
      for ( it = node->children.begin() ; it != node->children.end(); it++ ) {
        _renderNode(*it, dataStack, output);
      }
    }
    return;
  } else if( node->type == Node::TypeOutput ) {
    if( node->data != NULL && node->data->length() > 0 ) {
      output->append(*node->data);
    }
    return;
  }
  
  if( nstr == NULL ) {
    throw Exception("Whoops, empty tag");
  }
  
  // Resolve data
  Data * val = NULL;
  if( data->type == Data::TypeString ) {
    // Simple
    if( nstr->compare(".") == 0 ) {
      val = data;
    }
  } else if( data->type == Data::TypeMap ) {
    // Check top level
    Data::Map::iterator it = data->data.find(*nstr);
    if( it != data->data.end() ) {
      val = it->second;
    }
  }
  
  // Data was not resolved quickly
  if( val == NULL ) {
    // Search whole stack
    
    // Dot notation
    std::string initial(*nstr);
    std::vector<std::string> * parts = NULL;
    size_t found = initial.find(".");
    if( found != std::string::npos ) {
      parts = explode(".", initial);
      if( parts->size() > 0 ) {
        initial.assign(parts->front());
      }
    }
    
    // Resolve up the data stack
    Data * ref = NULL;
    Data::Map::iterator d_it;
    int i;
    Data ** dataStackPos = dataStack->end();
    for( i = 0; i < dataStack->size; i++, dataStackPos-- ) {
      if( (*dataStackPos)->type == Data::TypeMap ) {
        d_it = (*dataStackPos)->data.find(initial);
        if( d_it != (*dataStackPos)->data.end() ) {
          ref = d_it->second;
          if( ref != NULL ) {
            break;
          }
        }
      }
    }
    
    // Resolve or dot notation
    if( ref != NULL && parts != NULL && parts->size() > 1 ) {
      // Dot notation
      std::vector<std::string>::iterator vs_it;
      for( vs_it = parts->begin(), vs_it++; vs_it != parts->end(); vs_it++ ) {
        if( ref == NULL ) {
          break;
        } else if( ref->type != Data::TypeMap ) {
          ref = NULL; // Not sure about this
          break;
        } else {
          d_it = ref->data.find(*vs_it);
          if( d_it == ref->data.end() ) {
            ref = NULL; // Not sure about this
            break; 
          }
          ref = d_it->second;
        }
      }
    }
    
    if( ref != NULL ) {
      val = ref;
    }
  }
  
  // Calculate if value is empty
  bool valIsEmpty = true;
  if( val != NULL && !val->isEmpty() ) {
    valIsEmpty = false;
  }
  
  // Switch on node flags
  switch( node->flags ) {
    case Node::FlagComment:
    case Node::FlagStop:
    case Node::FlagInlinePartial:
      // Do nothing
      break;
    case Node::FlagNegate:
    case Node::FlagSection:
      // Negate/Empty list
      if( (node->flags & Node::FlagNegate) && !valIsEmpty ) {
        // Not-empty negation
        break;
      } else if( !(node->flags & Node::FlagNegate) && valIsEmpty ) {
        // Empty section
        break;
      } else if( node->children.size() <= 0 ) {
        // No children
        break;
      }
      
      if( valIsEmpty || val->type == Data::TypeString ) {
        Node::Children::iterator it;
        for( it = node->children.begin() ; it != node->children.end(); it++ ) {
          _renderNode(*it, dataStack, output);
        }
      } else if( val->type == Data::TypeList ) {
        // Numeric array/list
        Data::List::iterator childrenIt;
        Node::Children::iterator it;
        for ( childrenIt = val->children.begin() ; childrenIt != val->children.end(); childrenIt++ ) {
          dataStack->push(*childrenIt);
          for( it = node->children.begin() ; it != node->children.end(); it++ ) {
            _renderNode(*it, dataStack, output);
          }
          dataStack->pop();
        }
      } else if( val->type == Data::TypeArray ) {
        Data::Array ArrayPtr = val->array;
        int ArrayPos;
        Node::Children::iterator it;
        for ( ArrayPos = 0; ArrayPos < val->length; ArrayPos++, ArrayPtr++ ) {
          dataStack->push(ArrayPtr);
          for( it = node->children.begin() ; it != node->children.end(); it++ ) {
            _renderNode(*it, dataStack, output);
          }
          dataStack->pop();
        }
      } else if( val->type == Data::TypeMap ) {
        // Associate array/map
        Node::Children::iterator it;
        dataStack->push(val);
        for( it = node->children.begin() ; it != node->children.end(); it++ ) {
          _renderNode(*it, dataStack, output);
        }
        dataStack->pop();
      }
      break;
    case Node::FlagPartial:
      // Not yet implemented
      break;
    case Node::FlagEscape:
    case Node::FlagNone:
      if( !valIsEmpty && val->type == Data::TypeString ) {
        if( (bool) (node->flags & Node::FlagEscape) != true /*escapeByDefault*/ ) { // @todo escape by default
          // Probably shouldn't modify the value
          htmlspecialchars(val->val);
          output->append(*val->val);
        } else {
          output->append(*val->val);
        }
      }
      break;
    default:
      //php_error("Unknown node flags");
      break;
  }
}


} // namespace Mustache
