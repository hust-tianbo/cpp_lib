#include "cpp_lib/util/strings/replace.h"

namespace cpp_lib {
std::string StrReplaceAll(
    std::string_view str,
    std::initializer_list<std::pair<std::string_view, std::string_view>>
        replacements) {
  return absl::StrReplaceAll(str, replacements);
}

int StrReplaceAll(
    std::string* target,
    std::initializer_list<std::pair<std::string_view, std::string_view>>
        replacements) {
  return absl::StrReplaceAll(replacements, target);
}
}  // namespace cpp_lib