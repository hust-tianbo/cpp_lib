#include "cpp_lib/util/strings/trim.h"

#include <sstream>

constexpr char kWhiteSpace[] = " \n\r\t\f\v";

namespace cpp_lib {
std::string TrimLeftWhiteSpace(const std::string& str) {
  size_t start = str.find_first_not_of(kWhiteSpace);
  return (start == std::string::npos) ? "" : str.substr(start);
}

std::string TrimRightWhiteSpace(const std::string& str) {
  size_t end = str.find_last_not_of(kWhiteSpace);
  return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::string TrimWhiteSpace(const std::string& str) {
  if (str.empty()) {
    return str;
  }
  return TrimLeftWhiteSpace(TrimRightWhiteSpace(str));
}

std::string TrimEmptyLines(const std::string& str) {
  std::istringstream in(str);
  std::string line, text;
  while (std::getline(in, line)) {
    line = TrimRightWhiteSpace(TrimLeftWhiteSpace(line));
    if (line.empty()) {
      continue;
    }

    // 是否是注释
    if (line[0] == '#') {
      continue;
    }
    text.append(line).append("\n");
  }
  return text;
}

std::string TrimPrefixStr(const std::string& str, const std::string& prefix) {
  if (str.length() < prefix.length() ||
      str.substr(0, prefix.length()) != prefix) {
    return str;
  }
  return str.substr(prefix.length());
}

std::string TrimSuffixStr(const std::string& str, const std::string& suffix) {
  if (str.length() < suffix.length() ||
      str.substr(str.length() - suffix.length()) != suffix) {
    return str;
  }
  return str.substr(0, str.length() - suffix.length());
}

std::string TrimStr(const std::string& str, const std::string& trim_str) {
  if (str.empty()) {
    return str;
  }
  return TrimPrefixStr(TrimSuffixStr(str, trim_str), trim_str);
}

}  // namespace cpp_lib
