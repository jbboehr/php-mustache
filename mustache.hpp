
#ifndef MUSTACHE_HPP
#define MUSTACHE_HPP

#include <iostream>
#include <string>

class Mustache {
  private:
    std::string startSequence;
    std::string stopSequence;
  public:
    Mustache();
    ~Mustache();
    void setStartSequence(std::string start);
    void setStopSequence(std::string stop);
    std::string getStartSequence();
    std::string getStopSequence();
    //std::string render(std::string, std::map);
};


#endif
