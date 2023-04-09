#pragma once

#include <string>
#include <string_view>

namespace cpp_lib {
std::string TrimLeftWhiteSpace(const std::string& str);

std::string TrimRightWhiteSpace(const std::string& str);

std::string TrimWhiteSpace(const std::string& str);

std::string TrimEmptyLines(const std::string& str);

std::string TrimPrefixStr(const std::string& str, const std::string& prefix);

std::string TrimSuffixStr(const std::string& str, const std::string& suffix);

std::string TrimStr(const std::string& str, const std::string& trim_str);

}  // namespace cpp_lib