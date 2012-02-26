
#include "mustache.hpp"

Mustache::Mustache() {
  startSequence = "{{";
  stopSequence = "}}";
}

Mustache::~Mustache () {
  ;
}

void Mustache::setStartSequence(std::string start) {
  startSequence = start;
}

void Mustache::setStopSequence(std::string stop) {
  stopSequence = stop;
}

std::string Mustache::getStartSequence() {
  return startSequence;
}

std::string Mustache::getStopSequence() {
  return stopSequence;
}
