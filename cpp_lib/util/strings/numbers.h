#pragma once

#include <string_view>

#include "absl/strings/numbers.h"

namespace cpp_lib {
template <typename int_type>
[[nodiscard]] bool SimpleAtoi(std::string_view str, int_type* out) {
  return absl::SimpleAtoi(str, out);
}

[[nodiscard]] bool SimpleAtod(absl::string_view str, double* out) {
  return absl::SimpleAtod(str, out);
}

[[nodiscard]] bool SimpleAtob(absl::string_view str, bool* out) {
  return absl::SimpleAtob(str, out);
}

}  // namespace cpp_lib
