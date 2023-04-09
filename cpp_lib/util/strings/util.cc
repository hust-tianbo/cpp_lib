#include "cpp_lib/util/strings/util.h"

#include <time.h>

#include <string>
#include <string_view>

#include "absl/strings/ascii.h"

namespace cpp_lib {
std::string GetBaseName(const std::string& filename) {
#if defined(_WiIN32)
  char dir_sep('\\');
#else
  char dir_sep('/');
#endif

  std::string::size_type pos = filename.rfind(dir_sep);
  if (pos != std::string::npos) {
    return filename.substr(pos + 1);
  }
  return filename;
}

std::string StrToLower(std::string_view str) {
  return absl::AsciiStrToLower(str);
}

std::string StrToUpper(std::string_view str) {
  return absl::AsciiStrToUpper(str);
}

std::string RandomStr(uint32_t len) {
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  "abcdefghijklmnopqrstuvwxyz";

  srand(time(NULL));
  char buf[1024];
  for (uint32_t i = 0; i < len; i++) {
    buf[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }
  return std::string(buf, len);
}

}  // namespace cpp_lib