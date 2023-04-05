#pragma once

#include <string>

#include "absl/strings/str_cat.h"

namespace cpp_lib {
std::string StrCat();
std::string StrCat(const absl::AlphaNum& a);
}  // namespace cpp_lib