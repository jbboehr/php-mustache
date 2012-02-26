
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
    string * render(string * tmpl, HashTable * data_hash);
};


#endif
