#include "cpp_lib/util/strings/match.h"

#include <string_view>

#include "absl/strings/match.h"

namespace cpp_lib {
bool StrContain(std::string_view haystack, std::string_view needle) {
  return absl::StrContains(haystack, needle);
}

bool StrContain(std::string_view haystack, char needle) {
  return absl::StrContains(haystack, needle);
}

bool StartWith(std::string_view text, std::string_view prefix) {
  return absl::StartsWith(text, prefix);
}

bool EndWith(std::string_view text, std::string_view suffix) {
  return absl::EndsWith(text, suffix);
}

bool EqualIgnoreCase(std::string_view piece1, std::string_view piece2) {
  return absl::EqualsIgnoreCase(piece1, piece2);
}

bool StartWithIgnoreCase(std::string_view text, std::string_view prefix) {
  return absl::StartsWithIgnoreCase(text, prefix);
}

bool EndWithIgnoreCase(std::string_view text, std::string_view suffix) {
  return absl::EndsWithIgnoreCase(text, suffix);
}
}  // namespace cpp_lib