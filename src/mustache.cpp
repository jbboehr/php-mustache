
#include "mustache.hpp"

// TRIM
const std::string whiteSpaces( " \f\n\r\t\v" );


void trimRight( std::string& str,
      const std::string& trimChars = whiteSpaces )
{
   std::string::size_type pos = str.find_last_not_of( trimChars );
   str.erase( pos + 1 );    
}


void trimLeft( std::string& str,
      const std::string& trimChars = whiteSpaces )
{
   std::string::size_type pos = str.find_first_not_of( trimChars );
   str.erase( 0, pos );
}


void trim( std::string& str, const std::string& trimChars = whiteSpaces )
{
   trimRight( str, trimChars );
   trimLeft( str, trimChars );
}

void htmlspecialchars(string * str)
{
  // @todo switch this to php's version
  // this version is not utf-8 compatible
  string tmp;
  int pos = 0;
  int len = str->length();
  char * chr = (char *) str->c_str();
  tmp.reserve(len);
  for( pos = 0; pos < len; pos++ ) {
    switch( *chr ) {
      case '&':
        tmp.append("&amp;");
        break;
      case '"':
        tmp.append("&quot;");
        break;
      case '\'':
        tmp.append("&#039;");
        break;
      case '<':
        tmp.append("&lt;");
        break;
      case '>':
        tmp.append("&gt;");
        break;
      default:
        tmp.append(1, *chr);
        break;
    }
    chr++;
  }
  str->swap(tmp);
}

vector<string> * explode(const string &delimiter, const string &str)
{
  // http://www.zedwood.com/article/106/cpp-explode-function
  vector<string> * arr = new vector<string>;

  int strleng = str.length();
  int delleng = delimiter.length();
  if( delleng==0 ) {
    // no change
    return arr;
  }

  int i=0;
  int k=0;
  while( i < strleng ) {
      int j=0;
      while( i + j < strleng && j < delleng && str[i+j] == delimiter[j] ) {
        j++;
      }
      if( j == delleng ) {
        // found delimiter
        arr->push_back(str.substr(k, i-k));
        i += delleng;
        k = i;
      } else {
        i++;
      }
  }
  arr->push_back(str.substr(k, i-k));
  return arr;
}


// DATA

MustacheData::~MustacheData()
{
  // Val
  delete val;
  
  // Data
  if( data.size() > 0 ) {
    map<string,MustacheData*>::iterator dataIt;
    for ( dataIt = data.begin() ; dataIt != data.end(); dataIt++ ) {
      // (*dataIt).first; // Not a ref
      delete (*dataIt).second;
    }
  }
  data.clear();
  
  // Children
  if( children.size() > 0 ) {
    list<MustacheData *>::iterator childrenIt;
    for ( childrenIt = children.begin() ; childrenIt != children.end(); childrenIt++ ) {
      delete *childrenIt;
    }
  }
  children.clear();
}

int MustacheData::isEmpty()
{
  int ret = 0;
  switch( type ) {
    default:
    case MUSTACHE_DATA_NONE:
      ret = 1;
      break;
    case MUSTACHE_DATA_STRING:
      if( val == NULL || val->length() <= 0 ) {
        ret = 1;
      }
      break;
    case MUSTACHE_DATA_LIST:
      if( children.size() <= 0 ) {
        ret = 1;
      }
      break;
    case MUSTACHE_DATA_MAP:
      if( data.size() <= 0 ) {
        ret = 1;
      }
      break;
  }
  return ret;
}



// NODE

MustacheNode::~MustacheNode()
{
  // Data
  delete data;
  
  // Children
  if( children.size() > 0 ) {
    list<MustacheNode *>::iterator it;
    for ( it = children.begin() ; it != children.end(); it++ ) {
      delete *it;
    }
  }
  children.clear();
}



// MAIN

Mustache::Mustache() {
  startSequence = "{{";
  stopSequence = "}}";
  escapeByDefault = true;
}

Mustache::~Mustache () {
  ;
}

void Mustache::setStartSequence(string start) {
  startSequence = start;
}

void Mustache::setStopSequence(string stop) {
  stopSequence = stop;
}

std::string Mustache::getStartSequence() {
  return startSequence;
}

string Mustache::getStopSequence() {
  return stopSequence;
}

string * Mustache::render(string * tmpl, MustacheData * data) {
  MustacheNode * tree;
  string * return_val;
  
  // Tokenize template
  tree = tokenize(tmpl);
  
  // Render template
  return_val = renderTree(tree, data);
  
  return return_val;
}

MustacheNode * Mustache::tokenize(string * tmpl)
{
  unsigned int tmplL = tmpl->length();
  
  string start(startSequence);
  char startC = start.at(0);
  long startL = start.length();
  
  string stop(stopSequence);
  char stopC = stop.at(0);
  long stopL = stop.length();
  long tmpStopL = stopL;
  
  long pos = 0;
  const char * chr;
  long skipUntil = -1;
  long lineNo = 0;
  long charNo = 0;
  
  int inTag = 0;
  int inTripleTag = 0;
  int skip = 0;
  int currentFlags = 0;
  string buffer;
  buffer.reserve(100); // Reserver 100 chars
  
  int depth = 0;
  stack<MustacheNode *> nodeStack;
  MustacheNode * root;
  MustacheNode * node;
  
  // Initialize root node and stack[0]
  root = new MustacheNode;
  root->type = MUSTACHE_NODE_ROOT;
  root->flags = 0;
  root->data = NULL;
  
  nodeStack.push(root);
  
  // Scan loop
  chr = tmpl->c_str();
  for( pos = 0; pos < tmplL; pos++ ) {
    
    // Track line numbers
    if( *chr == '\n' ) {
      lineNo++;
      charNo = 0;
    } else {
      charNo++;
    }
      
    // Skip until position
    if( skipUntil > -1 ) {
      if( pos <= skipUntil ) {
        chr++;
        continue;
      } else {
        skipUntil = -1;
      }
    }
    
    // Main
    if( !inTag ) {
      if( *chr == startC && tmpl->compare(pos, startL, start) == 0 ) {
        // Close previous buffer
        if( buffer.length() > 0 ) {
          node = new MustacheNode();
          node->type = MUSTACHE_NODE_OUTPUT;
          node->data = new string(buffer);
          nodeStack.top()->children.push_back(node);
          buffer.clear();
        }
        // Open new buffer
        inTag = true;
        skipUntil = pos + startL - 1;
        // Triple mustache
        if( start.compare("{{") == 0 && tmpl->compare(pos+2, 1, "{") == 0 ) {
          inTripleTag = true;
          skipUntil++;
        }
      }
    } else {
      if( *chr == stopC && tmpl->compare(pos, stopL, stop) == 0 ) {
        // Trim buffer
        trim(buffer);
        if( buffer.length() <= 0 ) {
          throw MustacheException("Empty tag");
        }
        // Close and process previous buffer
        skip = false;
        tmpStopL = stopL;
        currentFlags = 0;
        switch( buffer.at(0) ) {
          case '&':
            currentFlags = MUSTACHE_FLAG_ESCAPE;
            break;
          case '^':
            currentFlags = MUSTACHE_FLAG_NEGATE;
            break;
          case '#':
            currentFlags = MUSTACHE_FLAG_SECTION;
            break;
          case '/':
            currentFlags = MUSTACHE_FLAG_STOP;
            break;
          case '!':
            currentFlags = MUSTACHE_FLAG_COMMENT;
            break;
          case '>':
            currentFlags = MUSTACHE_FLAG_PARTIAL;
            break;
          case '<':
            currentFlags = MUSTACHE_FLAG_INLINE_PARTIAL;
            break;
          case '=':
            throw MustacheException("Delimeters not yet supported");
            break;
        }
        if( !skip ) {
          if( currentFlags > 0 ) {
            buffer.erase(0, 1);
          }
          if( inTripleTag ) {
            currentFlags = currentFlags | MUSTACHE_FLAG_ESCAPE;
          }
          // Create node
          node = new MustacheNode();
          node->type = MUSTACHE_NODE_TAG;
          node->data = new string(buffer);
          node->flags = currentFlags;
          nodeStack.top()->children.push_back(node);
          // Push/pop stack
          if( currentFlags & MUSTACHE_CAN_HAVE_CHILDREN ) {
            depth++;
            nodeStack.push(node);
          } else if( currentFlags & MUSTACHE_FLAG_STOP ) {
            nodeStack.pop();
            depth--;
            if( depth < 0 ) {
              throw MustacheException("Extra closing section or missing opening section");
            }
          }
        }
        // Clear buffer
        buffer.clear();
        // Open new buffer
        inTag = false;
        skipUntil = pos + tmpStopL - 1;
        // Triple mustache
        if( !skip && inTripleTag && stop.compare("}}") == 0 ) {
          if( tmpl->compare(pos+2, 1, "}") != 0 ) {
            throw MustacheException("Missing closing triple mustache delimeter");
          }
          skipUntil++;
        }
      }
    }
    
    // Append to buffer
    if( skipUntil == -1 ) {
      buffer.append(1, *chr);
    }
    
    // Increment character pointer
    chr++;
  }
  
  if( inTag ) {
    throw MustacheException("Unclosed tag");
  } else if( buffer.length() > 0 ) {
    node = new MustacheNode();
    node->type = MUSTACHE_NODE_OUTPUT;
    node->data = new string(buffer);
    nodeStack.top()->children.push_back(node);
    buffer.clear();
  }
  
  return root;
}

string * Mustache::renderTree(MustacheNode * root, MustacheData * data)
{
  string * output = new string();
  output->reserve(MUSTACHE_OUTPUT_BUFFER_LENGTH);
  
  list<MustacheData*> * dataStack = new list<MustacheData*>();
  dataStack->push_back(data);
  
  _renderNode(root, dataStack, output);
  
  return output;
}

void Mustache::_renderNode(MustacheNode * node, list<MustacheData*> * dataStack, string * output)
{
  MustacheData * data = dataStack->back();
  string * nstr = node->data;
  
  if( data == NULL ) {
    throw MustacheException("Whoops, empty data");
  }
  
    // Handle simple cases
  if( node->type == MUSTACHE_NODE_ROOT ) {
    if( node->children.size() > 0 ) {
      list<MustacheNode *>::iterator it;
      for ( it = node->children.begin() ; it != node->children.end(); it++ ) {
        _renderNode(*it, dataStack, output);
      }
    }
    return;
  } else if( node->type == MUSTACHE_NODE_OUTPUT ) {
    if( node->data != NULL && node->data->length() > 0 ) {
      output->append(*node->data);
    }
    return;
  }
  
  if( nstr == NULL ) {
    throw MustacheException("Whoops, empty tag");
  }
  
  // Resolve data
  MustacheData * val = NULL;
  if( data->type == MUSTACHE_DATA_STRING ) {
    // Simple
    if( nstr->compare(".") == 0 ) {
      val = data;
    }
  } else if( data->type == MUSTACHE_DATA_MAP ) {
    // Check top level
    map<string,MustacheData *>::iterator it = data->data.find(*nstr);
    if( it != data->data.end() ) {
      val = it->second;
    }
  }
  
  // Data was not resolved quickly
  if( val == NULL ) {
    // Search whole stack
    
    // Dot notataion
    string initial(*nstr);
    vector<string> * parts = NULL;
    size_t found = initial.find(".");
    if( found != string::npos ) {
      parts = explode(".", initial);
      if( parts->size() > 0 ) {
        initial.assign(parts->front());
      }
    }
    
    // Resolve up the data stack
    MustacheData * ref = NULL;
    list<MustacheData*>::reverse_iterator ds_it;
    map<string,MustacheData *>::iterator d_it;
    for( ds_it = dataStack->rbegin() ; ds_it != dataStack->rend(); ds_it++ ) {
      if( (*ds_it)->type == MUSTACHE_DATA_MAP ) {
        d_it = (*ds_it)->data.find(initial);
        if( d_it != (*ds_it)->data.end() ) {
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
      vector<string>::iterator vs_it;
      for( vs_it = parts->begin(), vs_it++; vs_it != parts->end(); vs_it++ ) {
        if( ref == NULL ) {
          break;
        } else if( ref->type != MUSTACHE_DATA_MAP ) {
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
    case MUSTACHE_FLAG_COMMENT:
    case MUSTACHE_FLAG_STOP:
    case MUSTACHE_FLAG_INLINE_PARTIAL:
      // Do nothing
      break;
    case MUSTACHE_FLAG_NEGATE:
    case MUSTACHE_FLAG_SECTION:
      // Negate/Empty list
      if( (node->flags & MUSTACHE_FLAG_NEGATE) && !valIsEmpty ) {
        // Not-empty negation
        break;
      } else if( !(node->flags & MUSTACHE_FLAG_NEGATE) && valIsEmpty ) {
        // Empty section
        break;
      } else if( node->children.size() <= 0 ) {
        // No children
        break;
      }
      
      if( valIsEmpty || val->type == MUSTACHE_DATA_STRING ) {
        list<MustacheNode *>::iterator it;
        for( it = node->children.begin() ; it != node->children.end(); it++ ) {
          _renderNode(*it, dataStack, output);
        }
      } else if( val->type == MUSTACHE_DATA_LIST ) {
        // Numeric array/list
        list<MustacheData *>::iterator childrenIt;
        list<MustacheNode *>::iterator it;
        for ( childrenIt = val->children.begin() ; childrenIt != val->children.end(); childrenIt++ ) {
          dataStack->push_back(*childrenIt);
          for( it = node->children.begin() ; it != node->children.end(); it++ ) {
            _renderNode(*it, dataStack, output);
          }
          dataStack->pop_back();
        }
      } else if( val->type == MUSTACHE_DATA_MAP ) {
        // Associate array/map
        list<MustacheNode *>::iterator it;
        dataStack->push_back(val);
        for( it = node->children.begin() ; it != node->children.end(); it++ ) {
          _renderNode(*it, dataStack, output);
        }
        dataStack->pop_back();
      }
      break;
    case MUSTACHE_FLAG_PARTIAL:
      // Not yet implemented
      break;
    case MUSTACHE_FLAG_ESCAPE:
    case MUSTACHE_FLAG_NONE:
      if( !valIsEmpty && val->type == MUSTACHE_DATA_STRING ) {
        if( (bool) (node->flags & MUSTACHE_FLAG_ESCAPE) != escapeByDefault ) { // @todo escape by default
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