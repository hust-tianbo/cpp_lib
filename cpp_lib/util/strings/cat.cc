#include "cpp_lib/util/strings/cat.h"

namespace cpp_lib {
std::string StrCat() { return std::string(); }

std::string StrCat(const absl::AlphaNum& a) { return absl::StrCat(a); }
}  // namespace cpp_lib
