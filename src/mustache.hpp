
#ifndef MUSTACHE_HPP
#define MUSTACHE_HPP

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <stack>
#include <exception>
#include <vector>
#include <stdexcept>
#include <memory>

using namespace std;

const int MUSTACHE_OUTPUT_BUFFER_LENGTH = 100000;

class MustacheException : public runtime_error {
  public:
      MustacheException(const string& desc) : runtime_error(desc) { }
};

class MustacheData {
  public:
    typedef auto_ptr<MustacheData> Ptr;
    typedef string String;
    typedef map<string,MustacheData *> Map;
    typedef list<MustacheData *> List;
    typedef MustacheData * Array;
    enum Type { TypeNone = 0, TypeString = 1, TypeList = 2, TypeMap = 3, TypeArray = 4 };
    
    MustacheData::Type type;
    int length;
    MustacheData::String * val;
    MustacheData::Map data;
    MustacheData::List children;
    MustacheData::Array array;
    
    ~MustacheData();
    int isEmpty();
    void init(MustacheData::Type type, int size);
};

class MustacheNode {
  public:
    typedef auto_ptr<MustacheNode> Ptr;
    typedef vector<MustacheNode *> Children;
    enum Type { TypeNone = 0, TypeRoot = 1, TypeOutput = 2, TypeTag = 3 };
    enum Flag { 
      FlagNone = 0,
      FlagEscape = 1,
      FlagNegate = 2,
      FlagSection = 4,
      FlagStop = 8,
      FlagComment = 16,
      FlagPartial = 32,
      FlagInlinePartial = 64,
      
      FlagHasChildren = MustacheNode::FlagNegate | MustacheNode::FlagSection | MustacheNode::FlagPartial
    };
    
    MustacheNode::Type type;
    int flags;
    string * data;
    MustacheNode::Children children;
    
    ~MustacheNode();
};

class Mustache {
  private:
    string startSequence;
    string stopSequence;
    bool escapeByDefault;
    void _renderNode(MustacheNode * node, list<MustacheData*> * dataStack, string * output);
  public:
    typedef auto_ptr<Mustache> Ptr;
    Mustache();
    ~Mustache();
    void setStartSequence(string start);
    void setStopSequence(string stop);
    string getStartSequence();
    string getStopSequence();
    string * render(string * tmpl, MustacheData * data);
    MustacheNode * tokenize(string * tmpl);
    string * renderTree(MustacheNode * root, MustacheData * data);
};


#endif
