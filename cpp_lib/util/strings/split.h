#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "absl/strings/str_split.h"
#include "cpp_lib/util/strings/numbers.h"

namespace cpp_lib {
template <typename T = std::string>
std::vector<T> SplitStr(std::string_view str, std::string_view sep,
                        bool is_skip_empty = false,
                        bool is_skip_whitespace = false) {
  if (is_skip_whitespace) {
    return absl::StrSplit(str, sep, absl::SkipWhiteSpace());
  } else if (is_skip_empty) {
    return absl::StrSplit(str, sep, absl::SkipEmpty());
  } else {
    return absl::StrSplit(str, sep, absl::AllowEmpty());
  }
}

template <typename T = std::string>
std::vector<T> SplitStr(std::string_view str, char sep,
                        bool is_skip_empty = false,
                        bool is_skip_whitespace = false) {
  return SplitStr<T>(str, absl::ByChar(sep), is_skip_empty, is_skip_whitespace);
}

template <typename T>
std::vector<T> SplitAndToInt(std::string_view str, std::string_view sep,
                             bool is_skip_empty = false,
                             bool is_skip_whitespace = false) {
  std::vector<std::string> str_vec =
      SplitStr(str, sep, is_skip_empty, is_skip_whitespace);
  std::vector<T> res_vec;
  res_vec.reserve(str_vec.size());

  for (auto& str : str_vec) {
    T ele;
    if (SimpleAtoi(str, &ele)) {
      res_vec.emplace_back(ele);
    }
  }
  return std::move(res_vec);
}

template <typename T>
std::vector<T> SplitAndToInt(std::string_view str, char sep,
                             bool is_skip_empty = false,
                             bool is_skip_whitespace = false) {
  return SplitAndToInt(str, absl::ByChar(sep), is_skip_empty,
                       is_skip_whitespace);
}

}  // namespace cpp_lib