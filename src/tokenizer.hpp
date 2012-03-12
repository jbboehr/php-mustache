
#ifndef MUSTACHE_TOKENIZER_HPP
#define MUSTACHE_TOKENIZER_HPP

#include <memory>
#include <stack>
#include <string>

#include "exception.hpp"
#include "node.hpp"
#include "utils.hpp"

namespace mustache {


class Tokenizer {
  private:
    std::string _startSequence;
    std::string _stopSequence;
    bool _escapeByDefault;
    
  public:
    typedef std::auto_ptr<Tokenizer> Ptr;
    
    Tokenizer() : _startSequence("{{"), _stopSequence("}}"), _escapeByDefault(true) {};
    
    void setStartSequence(const std::string& start);
    void setStartSequence(const char * start);
    void setStopSequence(const std::string& stop);
    void setStopSequence(const char * stop);
    void setEscapeByDefault(bool flag);
    const std::string & getStartSequence();
    const std::string & getStopSequence();
    bool getEscapeByDefault();
    
    void tokenize(std::string * tmpl, Node * root);
};


} // namespace Mustache

#endif
