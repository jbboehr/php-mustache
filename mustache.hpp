
#ifndef MUSTACHE_HPP
#define MUSTACHE_HPP

extern "C" {
#include "php.h"
}

#include <iostream>
#include <string>
#include <map>
#include <list>

using namespace std;

class MustacheData {
  public:
    string val;
    map<string,MustacheData> dat;
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
    string * render(string * tmpl, map<string,MustacheData> * data);
};


#endif
