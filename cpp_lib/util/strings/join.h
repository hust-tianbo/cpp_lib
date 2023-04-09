#pragma once

#include <string_view>

#include "absl/strings/str_join.h"

namespace cpp_lib {
template <typename T>
std::string StrJoin(std::initializer_list<T> il, std::string_view sep) {
  return absl::StrJoin(il, sep);
}

template <typename Range>
std::string StrJoin(const Range& range, std::string_view sep) {
  return absl::StrJoin(range, sep);
}

template <typename Iterator>
std::string StrJoin(Iterator start, Iterator end, std::string_view sep) {
  return absl::StrJoin(start, end, sep);
}

}  // namespace cpp_lib