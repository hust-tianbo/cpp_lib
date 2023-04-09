#include "cpp_lib/util/http/url.h"

#include <ctype.h>

#include <vector>

#include "absl/strings/numbers.h"
#include "cpp_lib/util/strings/split.h"

namespace cpp_lib {
bool UrlDecode(const std::string& in, std::string& out) {
  out.clear();

  for (size_t i = 0; i < in.size(); i++) {
    char c = in[i];
    if (c == '+') {
      out.append(" ");
    } else if (c == '%' && (i + 2) < in.size() && isxdigit(in[i + 1]) &&
               isxdigit(in[i + 2])) {
      std::string hex_str;
      hex_str.append(1, in[i + 1]).append(1, in[i + 2]);
      unsigned int x;
      if (!absl::SimpleHexAtoi<unsigned int>(hex_str, &x)) {
        return false;
      }
      out.append(1, static_cast<char>(x));
      i += 2;
    }
  }
  return true;
}

bool Url2Map(const std::string& url,
             std::unordered_map<std::string, std::string>& args) {
  std::string v;
  if (!UrlDecode(url, v)) {
    return false;
  }

  std::vector<std::string> ss = SplitStr(v, '&');
  for (const std::string& item : ss) {
    if (item.empty()) {
      continue;
    }

    std::vector<std::string> kv = SplitStr(item, '=');
    if (kv.size() == 2) {
      args[kv[0]] = kv[1];
    }
  }
  return true;
}
}  // namespace cpp_lib