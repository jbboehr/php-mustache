
#ifndef MUSTACHE_UTILS_HPP
#define MUSTACHE_UTILS_HPP

#include <string>
#include <vector>

namespace mustache {



const std::string whiteSpaces(" \f\n\r\t\v");

const std::string specialChars("&\"'<>");

void trimRight(std::string& str, const std::string& trimChars = whiteSpaces);

void trimLeft(std::string& str, const std::string& trimChars = whiteSpaces);

void trim(std::string& str, const std::string& trimChars = whiteSpaces);

void htmlspecialchars(std::string * str);

std::vector<std::string> * explode(const std::string &delimiter, const std::string &str);

std::vector<std::string> * stringTok(const std::string &str, const std::string &delimiters);



} // namespace Mustache

#endif
