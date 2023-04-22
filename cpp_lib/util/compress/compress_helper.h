#pragma once

#include <string>

namespace cpp_lib {
int GzipString(const std::string& data, std::string& compressData, int leval);
int GunzipString(const std::string& compressData, std::string& data);

}  // namespace cpp_lib