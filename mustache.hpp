
#ifndef MUSTACHE_HPP
#define MUSTACHE_HPP

extern "C" {
#include "php.h"
}

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <stack>
#include <exception>

using namespace std;

const int MUSTACHE_DATA_NONE = 0;
const int MUSTACHE_DATA_STRING = 1;
const int MUSTACHE_DATA_LIST = 2;
const int MUSTACHE_DATA_MAP = 3;

const int MUSTACHE_NODE_ROOT = 1;
const int MUSTACHE_NODE_OUTPUT = 2;
const int MUSTACHE_NODE_TAG = 3;

const int MUSTACHE_FLAG_NONE = 0;
const int MUSTACHE_FLAG_ESCAPE = 1;
const int MUSTACHE_FLAG_NEGATE = 2;
const int MUSTACHE_FLAG_SECTION = 4;
const int MUSTACHE_FLAG_STOP = 8;
const int MUSTACHE_FLAG_COMMENT = 16;
const int MUSTACHE_FLAG_PARTIAL = 32;
const int MUSTACHE_FLAG_INLINE_PARTIAL = 64;

const int MUSTACHE_CAN_HAVE_CHILDREN = MUSTACHE_FLAG_SECTION | MUSTACHE_FLAG_NEGATE | MUSTACHE_FLAG_INLINE_PARTIAL;

class MustacheData {
  public:
    ~MustacheData();
    int type;
    string * val;
    map<string,MustacheData *> data;
    list<MustacheData *> children;
};

class MustacheNode {
  public:
    ~MustacheNode();
    int type;
    int flags;
    string * data;
    list<MustacheNode *> children;
};

class Mustache {
  private:
    string startSequence;
    string stopSequence;
  public:
    Mustache();
    ~Mustache();
    void setStartSequence(string start);
    void setStopSequence(string stop);
    string getStartSequence();
    string getStopSequence();
    string * render(string * tmpl, MustacheData * data);
    MustacheNode * tokenize(string * tmpl);
};


#endif
