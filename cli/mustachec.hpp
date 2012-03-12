
// Profiling:
// ./configure CPPFLAGS="-pg -g" CFLAGS="-pg -g" CXXFLAGS="-pg -g"

#ifndef MUSTACHEC_HPP
#define MUSTACHEC_HPP

#include <iostream>
#include <string>

extern "C" {
  #include <ctype.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>

  #include "json.h"
  #include "jsonlint.h"
}

#include "mustache.hpp"

using namespace std;
using namespace mustache;

#endif
