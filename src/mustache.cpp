
#include "mustache.hpp"

// TRIM
const std::string whiteSpaces(" \f\n\r\t\v");
const std::string specialChars("&\"'<>");

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
  tmp.reserve(len * 2);
  for( pos = 0; pos < len; pos++, chr++ ) {
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

vector<string> * stringTok(const string &str, const string &delimiters)
{
  // http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
  vector<string> * tokens = new vector<string>;
  
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  
  // Find first "non-delimiter".
  string::size_type pos = str.find_first_of(delimiters, lastPos);

  while( string::npos != pos || string::npos != lastPos ) {
    // Found a token, add it to the vector.
    tokens->push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
  
  return tokens;
}


// DATA

MustacheData::~MustacheData()
{
  switch( this->type ) {
    case MustacheData::TypeString:
      delete val;
      break;
    case MustacheData::TypeMap:
      if( data.size() > 0 ) {
        MustacheData::Map::iterator dataIt;
        for ( dataIt = data.begin() ; dataIt != data.end(); dataIt++ ) {
          delete (*dataIt).second;
        }
        data.clear();
      }
      break;
    case MustacheData::TypeList:
      if( children.size() > 0 ) {
        MustacheData::List::iterator childrenIt;
        for ( childrenIt = children.begin() ; childrenIt != children.end(); childrenIt++ ) {
          delete *childrenIt;
        }
        children.clear();
      }
    case MustacheData::TypeArray:
      delete[] array;
      break;
  }
}

void MustacheData::init(MustacheData::Type type, int size) {
  this->type = type;
  this->length = size;
  switch( type ) {
    case MustacheData::TypeString:
      val = new string();
      val->reserve(size);
      break;
    case MustacheData::TypeList:
      // Do nothing
      break;
    case MustacheData::TypeMap:
      // Do nothing
      break;
    case MustacheData::TypeArray:
      this->array = new MustacheData[size];
      break;
  }
};

int MustacheData::isEmpty()
{
  int ret = 0;
  switch( type ) {
    default:
    case MustacheData::TypeNone:
      ret = 1;
      break;
    case MustacheData::TypeString:
      if( val == NULL || val->length() <= 0 ) {
        ret = 1;
      }
      break;
    case MustacheData::TypeList:
      if( children.size() <= 0 ) {
        ret = 1;
      }
      break;
    case MustacheData::TypeMap:
      if( data.size() <= 0 ) {
        ret = 1;
      }
      break;
    case MustacheData::TypeArray:
      if( length <= 0 ) {
        ret = 1;
      }
      break;
  }
  return ret;
}



// DATA STACK
void MustacheDataStack::push(MustacheData * data)
{
  if( this->size < 0 || this->size >= MUSTACHE_DATA_STACK_SIZE ) {
    throw MustacheException("Reached max stack size");
  }
  this->stack[this->size] = data;
  this->size++;
}

void MustacheDataStack::pop()
{
  if( this->size > 0 ) {
    this->size--;
    this->stack[this->size] = NULL;
  }
}

MustacheData * MustacheDataStack::top()
{
  if( this->size <= 0 ) {
    throw MustacheException("Reached bottom of stack");
  } else {
    return this->stack[this->size - 1];
  }
}



// NODE

MustacheNode::~MustacheNode()
{
  // Data
  delete data;
  
  // Children
  if( children.size() > 0 ) {
    MustacheNode::Children::iterator it;
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
  int currentFlags = MustacheNode::FlagNone;
  string buffer;
  buffer.reserve(100); // Reserver 100 chars
  
  int depth = 0;
  stack<MustacheNode *> nodeStack;
  MustacheNode * root;
  MustacheNode * node;
  
  // Initialize root node and stack[0]
  root = new MustacheNode;
  root->type = MustacheNode::TypeRoot;
  root->flags = MustacheNode::FlagNone;
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
          node->type = MustacheNode::TypeOutput;
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
        currentFlags = MustacheNode::FlagNone;
        switch( buffer.at(0) ) {
          case '&':
            currentFlags = MustacheNode::FlagEscape;
            break;
          case '^':
            currentFlags = MustacheNode::FlagNegate;
            break;
          case '#':
            currentFlags = MustacheNode::FlagSection;
            break;
          case '/':
            currentFlags = MustacheNode::FlagStop;
            break;
          case '!':
            currentFlags = MustacheNode::FlagComment;
            break;
          case '>':
            currentFlags = MustacheNode::FlagPartial;
            break;
          case '<':
            currentFlags = MustacheNode::FlagInlinePartial;
            break;
          case '=':
            if( buffer.at(buffer.length()-1) != '=' ) {
              throw MustacheException("Missing closing delimiter (=)");
            }
            trim(buffer, " \f\n\r\t\v=");
            
            vector<string> * delims = stringTok(buffer, whiteSpaces);
            if( delims->size() != 2 || delims[0].size() < 1 || delims[1].size() < 1 ) {
              delete delims;
              throw MustacheException("Invalid delimiter format");
            }
            
            // Assign new start/stop
            start.assign(delims->at(0));
            startC = start.at(0);
            startL = start.length();
            stop.assign(delims->at(1));
            stopC = stop.at(0);
            stopL = stop.length();
            skip = 1;
            break;
        }
        if( !skip ) {
          if( currentFlags > 0 ) {
            buffer.erase(0, 1);
          }
          if( inTripleTag ) {
            currentFlags = currentFlags | MustacheNode::FlagEscape;
          }
          // Create node
          node = new MustacheNode();
          node->type = MustacheNode::TypeTag;
          node->data = new string(buffer);
          node->flags = currentFlags;
          nodeStack.top()->children.push_back(node);
          // Push/pop stack
          if( currentFlags & MustacheNode::FlagHasChildren ) {
            depth++;
            nodeStack.push(node);
          } else if( currentFlags & MustacheNode::FlagStop ) {
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
            throw MustacheException("Missing closing triple mustache delimiter");
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
    node->type = MustacheNode::TypeOutput;
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
  
  MustacheDataStack * dataStack = new MustacheDataStack();
  dataStack->push(data);
  
  _renderNode(root, dataStack, output);
  
  return output;
}

void Mustache::_renderNode(MustacheNode * node, MustacheDataStack * dataStack, string * output)
{
  MustacheData * data = dataStack->top();
  string * nstr = node->data;
  
  if( data == NULL ) {
    throw MustacheException("Whoops, empty data");
  }
  
    // Handle simple cases
  if( node->type == MustacheNode::TypeRoot ) {
    if( node->children.size() > 0 ) {
      MustacheNode::Children::iterator it;
      for ( it = node->children.begin() ; it != node->children.end(); it++ ) {
        _renderNode(*it, dataStack, output);
      }
    }
    return;
  } else if( node->type == MustacheNode::TypeOutput ) {
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
  if( data->type == MustacheData::TypeString ) {
    // Simple
    if( nstr->compare(".") == 0 ) {
      val = data;
    }
  } else if( data->type == MustacheData::TypeMap ) {
    // Check top level
    map<string,MustacheData *>::iterator it = data->data.find(*nstr);
    if( it != data->data.end() ) {
      val = it->second;
    }
  }
  
  // Data was not resolved quickly
  if( val == NULL ) {
    // Search whole stack
    
    // Dot notation
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
    map<string,MustacheData *>::iterator d_it;
    int i;
    MustacheData ** dataStackPos = dataStack->stack + dataStack->size - 1;
    for( i = 0; i < dataStack->size; i++, dataStackPos-- ) {
      if( (*dataStackPos)->type == MustacheData::TypeMap ) {
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
      vector<string>::iterator vs_it;
      for( vs_it = parts->begin(), vs_it++; vs_it != parts->end(); vs_it++ ) {
        if( ref == NULL ) {
          break;
        } else if( ref->type != MustacheData::TypeMap ) {
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
    case MustacheNode::FlagComment:
    case MustacheNode::FlagStop:
    case MustacheNode::FlagInlinePartial:
      // Do nothing
      break;
    case MustacheNode::FlagNegate:
    case MustacheNode::FlagSection:
      // Negate/Empty list
      if( (node->flags & MustacheNode::FlagNegate) && !valIsEmpty ) {
        // Not-empty negation
        break;
      } else if( !(node->flags & MustacheNode::FlagNegate) && valIsEmpty ) {
        // Empty section
        break;
      } else if( node->children.size() <= 0 ) {
        // No children
        break;
      }
      
      if( valIsEmpty || val->type == MustacheData::TypeString ) {
        MustacheNode::Children::iterator it;
        for( it = node->children.begin() ; it != node->children.end(); it++ ) {
          _renderNode(*it, dataStack, output);
        }
      } else if( val->type == MustacheData::TypeList ) {
        // Numeric array/list
        MustacheData::List::iterator childrenIt;
        MustacheNode::Children::iterator it;
        for ( childrenIt = val->children.begin() ; childrenIt != val->children.end(); childrenIt++ ) {
          dataStack->push(*childrenIt);
          for( it = node->children.begin() ; it != node->children.end(); it++ ) {
            _renderNode(*it, dataStack, output);
          }
          dataStack->pop();
        }
      } else if( val->type == MustacheData::TypeArray ) {
        MustacheData::Array ArrayPtr = val->array;
        int ArrayPos;
        MustacheNode::Children::iterator it;
        for ( ArrayPos = 0; ArrayPos < val->length; ArrayPos++, ArrayPtr++ ) {
          dataStack->push(ArrayPtr);
          for( it = node->children.begin() ; it != node->children.end(); it++ ) {
            _renderNode(*it, dataStack, output);
          }
          dataStack->pop();
        }
      } else if( val->type == MustacheData::TypeMap ) {
        // Associate array/map
        MustacheNode::Children::iterator it;
        dataStack->push(val);
        for( it = node->children.begin() ; it != node->children.end(); it++ ) {
          _renderNode(*it, dataStack, output);
        }
        dataStack->pop();
      }
      break;
    case MustacheNode::FlagPartial:
      // Not yet implemented
      break;
    case MustacheNode::FlagEscape:
    case MustacheNode::FlagNone:
      if( !valIsEmpty && val->type == MustacheData::TypeString ) {
        if( (bool) (node->flags & MustacheNode::FlagEscape) != escapeByDefault ) { // @todo escape by default
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