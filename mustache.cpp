
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


// MAIN

Mustache::Mustache() {
  startSequence = "{{";
  stopSequence = "}}";
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

string * Mustache::render(string * tmpl, map<string,MustacheData> * data) {
  MustacheNode * tree;
  string * return_val = new string();
  
  // Tokenize template
  tree = tokenize(tmpl);
  
  
  return return_val;
}

MustacheNode * Mustache::tokenize(string * tmpl)
{
  uint tmplL = tmpl->length();
  
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
    if( chr == "\n" ) {
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
          throw new exception(); // @todo fix this
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
            throw new exception(); // @todo fix this
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
          nodeStack.top()->children.push_back(node);
          // Push/pop stack
          if( currentFlags & MUSTACHE_CAN_HAVE_CHILDREN ) {
            depth++;
            nodeStack.push(node);
          } else if( currentFlags & MUSTACHE_FLAG_STOP ) {
            nodeStack.pop();
            depth--;
            if( depth < 0 ) {
              throw new exception(); // @todo fix this
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
            throw new exception(); // @todo fix this
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
    throw new exception(); // @todo fix this
  } else if( buffer.length() > 0 ) {
    node = new MustacheNode();
    node->type = MUSTACHE_NODE_OUTPUT;
    node->data = new string(buffer);
    nodeStack.top()->children.push_back(node);
    buffer.clear();
  }
  
  return root;
}