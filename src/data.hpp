
#include <list>
#include <memory>
#include <map>
#include <string>
#include <vector>

#include "exception.hpp"

#ifndef MUSTACHE_DATA_HPP
#define MUSTACHE_DATA_HPP

namespace mustache {

  
class Data {
  public:
    typedef std::auto_ptr<Data> Ptr;
    typedef std::string String;
    typedef std::map<std::string,Data *> Map;
    typedef std::list<Data *> List;
    typedef Data * Array;
    enum Type { 
      TypeNone = 0,
      TypeString = 1,
      TypeList = 2,
      TypeMap = 3,
      TypeArray = 4
    };
    
    Data::Type type;
    int length;
    Data::String * val;
    Data::Map data;
    Data::List children;
    Data::Array array;
    
    ~Data();
    int isEmpty();
    void init(Data::Type type, int size);
};


class Stack {
  public:
    static const int MAXSIZE = 100;
    
    Stack() : size(0) {};
    int size;
    Data * stack[Stack::MAXSIZE];
    void push(Data * data);
    void pop();
    Data * top();
    Data ** begin();
    Data ** end();
};


} // namespace Mustache

#endif


