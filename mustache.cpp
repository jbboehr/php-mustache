
#include "mustache.hpp"


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
  
  long pos = 0;
  const char * chr;
  long skipUntil = -1;
  long lineNo = 0;
  long charNo = 0;
  
  int inTag = 0;
  int inTripleTag = 0;
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
      if( pos < skipUntil ) {
        chr++;
        continue;
      } else {
        skipUntil = -1;
      }
    }
    
    // Main
    if( !inTag ) {
      if( *chr == startC ) {
        
      }
    } else {
      if( *chr == stopC ) {
        
      }
    }
    
    // Append to buffer
    buffer.append(chr, (size_t) 1);
    
    // Increment character pointer
    chr++;
  }
}