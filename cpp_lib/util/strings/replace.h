#pragma once

#include <initializer_list>
#include <map>
#include <string>
#include <string_view>

#include "absl/strings/str_replace.h"

namespace cpp_lib {
std::string StrReplaceAll(
    std::string_view str,
    std::initializer_list<std::pair<std::string_view, std::string_view>>
        replacements);

template <typename StrToStrMapping>
std::string StrReplaceAll(std::string_view str,
                          const StrToStrMapping& replacements) {
  return absl::StrReplaceAll(str, replacements);
}

int StrReplaceAll(
    std::string* target,
    std::initializer_list<std::pair<std::string_view, std::string_view>>
        replacements);
}  // namespace cpp_lib